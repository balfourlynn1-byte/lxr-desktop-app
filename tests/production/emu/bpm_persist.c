/* Cold-start BPM persistence through the production preset manager and FatFs. */
#include "lxr_api.h"
#include "Parameters.h"
#include "Preset/PresetManager.h"
#include "Soundengine/Sequencer/sequencer.h"
#include "emu_hw.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int save_card(const char *path)
{
    FILE *out = fopen(path, "wb");
    if (!out) return 1;
    uint32_t header[2] = {lxr_sd_num_chunks(), lxr_sd_chunk_size()};
    if (fwrite(header, sizeof header, 1, out) != 1) return 1;
    for (uint32_t i = 0; i < header[0]; ++i) {
        const uint8_t *chunk = lxr_sd_chunk(i);
        if (chunk && (fwrite(&i, sizeof i, 1, out) != 1 ||
                      fwrite(chunk, header[1], 1, out) != 1)) return 1;
    }
    return fclose(out) != 0;
}

static int load_card(const char *path)
{
    FILE *in = fopen(path, "rb");
    if (!in) return 1;
    uint32_t header[2];
    if (fread(header, sizeof header, 1, in) != 1 ||
        header[0] != lxr_sd_num_chunks() || header[1] != lxr_sd_chunk_size()) return 1;
    uint32_t i;
    while (fread(&i, sizeof i, 1, in) == 1) {
        if (i >= header[0] ||
            fread(lxr_sd_chunk_for_load(i), header[1], 1, in) != 1) return 1;
    }
    if (!feof(in)) return 1;
    return fclose(in) != 0;
}

int main(int argc, char **argv)
{
    if (argc != 3 || lxr_sd_create(128) != 0) return 2;
    if (!strcmp(argv[1], "save") || !strcmp(argv[1], "save_explicit")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_idle_blocks(1600);
        lxr_set_param(PAR_BPM, 137);
        lxr_idle_blocks(1600); /* allow the production delayed-save timer */
        printf("before restart: parameter=%d runtime=%u\n", lxr_get_param(PAR_BPM), seq_getBpm());
        if (lxr_get_param(PAR_BPM) != 137 || seq_getBpm() != 137) return 3;
        if (!strcmp(argv[1], "save_explicit")) {
            if (preset_saveGlobals() != FR_OK) return 3;
            lxr_set_param(PAR_BPM, 120);
            emu_sd_set_present(0);
            FRESULT failed_save = preset_saveGlobals();
            printf("removed-card globals save: FatFs=%d\n", failed_save);
            if (failed_save == FR_OK) return 3;
            emu_sd_set_present(1);
            if (!preset_loadGlobals()) return 3;
            lxr_idle_blocks(32);
            printf("same-process reload: parameter=%d runtime=%u\n",
                   lxr_get_param(PAR_BPM), seq_getBpm());
            if (lxr_get_param(PAR_BPM) != 137 || seq_getBpm() != 137) return 3;
        }
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "verify")) {
        if (load_card(argv[2]) || lxr_boot(123) != 0) return 2;
        printf("after restart: parameter=%d runtime=%u\n", lxr_get_param(PAR_BPM), seq_getBpm());
        return (lxr_get_param(PAR_BPM) == 137 && seq_getBpm() == 137) ? 0 : 4;
    }
    return 2;
}
