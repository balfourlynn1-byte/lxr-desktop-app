/* Cold-start BPM persistence through the production preset manager and FatFs. */
#include "lxr_api.h"
#include "Parameters.h"
#include "Preset/PresetManager.h"
#include "Soundengine/Sequencer/sequencer.h"
#include "menu/menu.h"
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
    if (!strcmp(argv[1], "save") || !strcmp(argv[1], "save_explicit") ||
        !strcmp(argv[1], "save_fault") || !strcmp(argv[1], "save_corrupt") ||
        !strcmp(argv[1], "save_full")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_idle_blocks(1600);
        lxr_set_param(PAR_BPM, 137);
        lxr_idle_blocks(1600); /* allow the production delayed-save timer */
        printf("before restart: parameter=%d runtime=%u\n", lxr_get_param(PAR_BPM), seq_getBpm());
        if (lxr_get_param(PAR_BPM) != 137 || seq_getBpm() != 137) return 3;
        if (!strcmp(argv[1], "save_explicit") || !strcmp(argv[1], "save_fault") ||
            !strcmp(argv[1], "save_corrupt") || !strcmp(argv[1], "save_full")) {
            if (strcmp(argv[1], "save_explicit")) lxr_set_param(PAR_MIDI_CHAN_GLOBAL, 2);
            if (preset_saveGlobals() != FR_OK) return 3;
            if (!strcmp(argv[1], "save_full")) {
                FIL filler;
                uint8_t block[65536] = {0};
                if (f_open(&filler, "/fill.dat", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) return 3;
                unsigned long total = 0;
                for (;;) {
                    UINT written = 0;
                    FRESULT result = f_write(&filler, block, sizeof block, &written);
                    total += written;
                    if (result != FR_OK || written != sizeof block) break;
                    if (total > 128ul * 1024ul * 1024ul) return 3;
                }
                if (f_close(&filler) != FR_OK) return 3;
                printf("filled virtual SD bytes=%lu\n", total);
                lxr_set_param(PAR_BPM, 145);
                lxr_set_param(PAR_MIDI_CHAN_GLOBAL, 5);
                FRESULT full_save = preset_saveGlobals();
                printf("full-card globals save: FatFs=%d\n", full_save);
                if (full_save == FR_OK) return 5;
                return save_card(argv[2]);
            }
            if (!strcmp(argv[1], "save_corrupt")) {
                lxr_set_param(PAR_BPM, 145);
                lxr_set_param(PAR_MIDI_CHAN_GLOBAL, 5);
                if (preset_saveGlobals() != FR_OK) return 3;
                char path[sizeof(currentFolder) + sizeof("/glo.cfg")];
                snprintf(path, sizeof path, "%s/glo.cfg", currentFolder);
                FIL file;
                if (f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK ||
                    f_close(&file) != FR_OK) return 3;
                return save_card(argv[2]);
            }
            if (!strcmp(argv[1], "save_fault")) {
                lxr_set_param(PAR_BPM, 145);
                lxr_set_param(PAR_MIDI_CHAN_GLOBAL, 5);
                const char *write_at = getenv("JB_FAIL_WRITE");
                const char *sync_at = getenv("JB_FAIL_SYNC");
                emu_sd_fault(write_at ? (uint32_t)atoi(write_at) : 0,
                             sync_at ? (uint32_t)atoi(sync_at) : 0);
                FRESULT failed_save = preset_saveGlobals();
                emu_sd_fault(0, 0);
                printf("injected save failure: FatFs=%d\n", failed_save);
                if (failed_save == FR_OK) return 5;
                return save_card(argv[2]);
            }
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
    if (!strcmp(argv[1], "verify") || !strcmp(argv[1], "verify_fault") ||
        !strcmp(argv[1], "verify_old")) {
        if (load_card(argv[2]) || lxr_boot(123) != 0) return 2;
        printf("after restart: parameter=%d runtime=%u global_channel=%d project=%u\n",
               lxr_get_param(PAR_BPM), seq_getBpm(),
               lxr_get_param(PAR_MIDI_CHAN_GLOBAL), menu_loadedPresetNr[CURRENT_PROJECT]);
        if (!strcmp(argv[1], "verify_fault")) {
            int bpm = lxr_get_param(PAR_BPM);
            return (menu_loadedPresetNr[CURRENT_PROJECT] == 0 &&
                    ((bpm == 137 && lxr_get_param(PAR_MIDI_CHAN_GLOBAL) == 2) ||
                     (bpm == 145 && lxr_get_param(PAR_MIDI_CHAN_GLOBAL) == 5)) &&
                    seq_getBpm() == bpm) ? 0 : 4;
        }
        if (!strcmp(argv[1], "verify_old"))
            return (lxr_get_param(PAR_BPM) == 137 && seq_getBpm() == 137 &&
                    lxr_get_param(PAR_MIDI_CHAN_GLOBAL) == 2 &&
                    menu_loadedPresetNr[CURRENT_PROJECT] == 0) ? 0 : 4;
        return (lxr_get_param(PAR_BPM) == 137 && seq_getBpm() == 137) ? 0 : 4;
    }
    return 2;
}
