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

static void set_project_generation(int newer)
{
    memcpy(savefileNames[0][PROJECT_NAMES], newer ? "NEWPROJ " : "OLDPROJ ", 8);
    memcpy(savefileNames[0][KIT_NAMES], newer ? "NEWKIT  " : "OLDKIT  ", 8);
    memcpy(savefileNames[0][PATTERN_NAMES], newer ? "NEWPAT  " : "OLDPAT  ", 8);
    memcpy(savefileNames[0][SONG_NAMES], newer ? "NEWSONG " : "OLDSONG ", 8);
    seq_patternSet.shuffle[0] = newer ? 33 : 11;
    seq_songData[0][0] = newer ? 44 : 22;
}

static int check_project_generation(int newer)
{
    FILINFO info;
    const char *prefix = newer ? "NEW" : "OLD";
    const char *other = newer ? "OLD" : "NEW";
    const char *extensions[] = {"KI.snd", "PA.pat", "SO.sng"};
    char path[32];
    for (unsigned i = 0; i < 3; ++i) {
        snprintf(path, sizeof path, "/proj00/00-%.3s%s", prefix, extensions[i]);
        FRESULT found = f_stat(path, &info);
        if (found != FR_OK || info.fsize <= 8) {
            printf("generation missing %s status=%d\n", path, found);
            return 1;
        }
        snprintf(path, sizeof path, "/proj00/00-%.3s%s", other, extensions[i]);
        if (f_stat(path, &info) == FR_OK) {
            printf("generation stale %s\n", path);
            return 1;
        }
    }
    if (f_stat("/proj00/tx.ok", &info) != FR_OK || info.fsize != 20) {
        printf("generation marker missing\n");
        return 1;
    }
    FIL file;
    UINT read = 0;
    char name[8];
    if (f_open(&file, "/proj00/prj.nfo", FA_READ) != FR_OK) return 1;
    FRESULT result = f_read(&file, name, sizeof name, &read);
    FRESULT close_result = f_close(&file);
    if (result != FR_OK || close_result != FR_OK || read != sizeof name ||
        memcmp(name, newer ? "NEWPROJ " : "OLDPROJ ", 8)) return 1;
    printf("generation values pattern=%u song=%u\n", seq_patternSet.shuffle[0], seq_songData[0][0]);
    return seq_patternSet.shuffle[0] != (newer ? 33 : 11) ||
           seq_songData[0][0] != (newer ? 44 : 22);
}

static int fill_virtual_card(void)
{
    FIL filler;
    uint8_t block[65536] = {0};
    if (f_open(&filler, "/fill.dat", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) return 1;
    unsigned long total = 0;
    for (;;) {
        UINT written = 0;
        FRESULT result = f_write(&filler, block, sizeof block, &written);
        total += written;
        if (result != FR_OK || written != sizeof block) break;
        if (total > 128ul * 1024ul * 1024ul) return 1;
    }
    if (f_close(&filler) != FR_OK) return 1;
    printf("filled virtual SD bytes=%lu\n", total);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3 || lxr_sd_create(128) != 0) return 2;
    if (!strcmp(argv[1], "contexts")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_idle_blocks(1600);
        lxr_set_param(PAR_BPM, 137);
        if (preset_saveProject(0) != FR_OK) return 3;
        lxr_set_param(PAR_BPM, 145);
        if (preset_saveProject(1) != FR_OK) return 3;
        if (preset_loadProject(0, 1) != FR_OK) return 3;
        lxr_idle_blocks(32);
        printf("project 0 tempo=%d/%u\n", lxr_get_param(PAR_BPM), seq_getBpm());
        if (lxr_get_param(PAR_BPM) != 137 || seq_getBpm() != 137) return 4;
        if (preset_loadProject(1, 1) != FR_OK) return 3;
        lxr_idle_blocks(32);
        printf("project 1 tempo=%d/%u\n", lxr_get_param(PAR_BPM), seq_getBpm());
        if (lxr_get_param(PAR_BPM) != 145 || seq_getBpm() != 145) return 4;
        preset_loadDrumsetRam(0, 0, 1);
        lxr_idle_blocks(32);
        if (lxr_get_param(PAR_BPM) != 145 || seq_getBpm() != 145) return 4;
        lxr_set_param(PAR_BPM, 160); /* unsaved edit must not replace project BPM */
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "save_sync")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_set_param(PAR_BPM, 0); /* external trigger clock */
        if (preset_saveGlobals() != FR_OK) return 3;
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "project_repeat")) {
        if (lxr_boot(123) != 0) return 2;
        /* Three successful replacements exercise deletion of an older /bak00.
         * FatFs rejects rename when the destination directory still exists. */
        const int tempos[] = {137, 145, 160};
        for (unsigned i = 0; i < 3; ++i) {
            int bpm = tempos[i];
            lxr_set_param(PAR_BPM, bpm);
            FRESULT result = preset_saveProject(0);
            printf("project replacement BPM=%d FatFs=%d\n", bpm, result);
            if (result != FR_OK) return 3;
        }
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "project_measure")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_set_param(PAR_BPM, 137);
        if (preset_saveProject(0) != FR_OK) return 3;
        lxr_set_param(PAR_BPM, 145);
        emu_sd_fault(UINT32_MAX, 0); /* count writes without a reachable failure */
        if (preset_saveProject(0) != FR_OK) return 3;
        printf("%u\n", emu_sd_write_count());
        return 0;
    }
    if (!strcmp(argv[1], "project_fault")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_set_param(PAR_BPM, 137);
        set_project_generation(0);
        if (preset_saveProject(0) != FR_OK) return 3;
        lxr_set_param(PAR_BPM, 145);
        set_project_generation(1);
        const char *write_at = getenv("JB_FAIL_WRITE");
        const char *sync_at = getenv("JB_FAIL_SYNC");
        if (!write_at && !sync_at) return 2;
        emu_sd_fault(write_at ? (uint32_t)atoi(write_at) : 0,
                     sync_at ? (uint32_t)atoi(sync_at) : 0);
        FRESULT result = preset_saveProject(0);
        uint32_t writes = emu_sd_write_count();
        emu_sd_fault(0, 0);
        printf("project save fault write=%s sync=%s FatFs=%d calls=%u\n",
               write_at ? write_at : "0", sync_at ? sync_at : "0", result, writes);
        if (result == FR_OK) return 5;
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "project_full")) {
        if (lxr_boot(123) != 0) return 2;
        lxr_set_param(PAR_BPM, 137);
        set_project_generation(0);
        if (preset_saveProject(0) != FR_OK || fill_virtual_card()) return 3;
        lxr_set_param(PAR_BPM, 145);
        set_project_generation(1);
        FRESULT result = preset_saveProject(0);
        printf("full-card project save FatFs=%d\n", result);
        if (result == FR_OK) return 5;
        return save_card(argv[2]);
    }
    if (!strcmp(argv[1], "project_retry")) {
        if (load_card(argv[2]) || lxr_boot(123) != 0) return 2;
        FILINFO info;
        if (f_stat("/fill.dat", &info) == FR_OK && f_unlink("/fill.dat") != FR_OK) return 3;
        lxr_set_param(PAR_BPM, 145);
        set_project_generation(1);
        FRESULT result = preset_saveProject(0);
        printf("retry project save FatFs=%d\n", result);
        if (result != FR_OK) return 3;
        return save_card(argv[2]);
    }
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
                if (fill_virtual_card()) return 3;
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
        !strcmp(argv[1], "verify_old") || !strcmp(argv[1], "verify_context") ||
        !strcmp(argv[1], "verify_sync") || !strcmp(argv[1], "verify_project") || !strcmp(argv[1], "verify_repeat")) {
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
        if (!strcmp(argv[1], "verify_context"))
            return (lxr_get_param(PAR_BPM) == 145 && seq_getBpm() == 145 &&
                    menu_loadedPresetNr[CURRENT_PROJECT] == 1) ? 0 : 4;
        if (!strcmp(argv[1], "verify_sync")) {
            printf("external sync mode=%u\n", seq_getExtSync());
            return (lxr_get_param(PAR_BPM) == 0 && seq_getExtSync() == 1) ? 0 : 4;
        }
        if (!strcmp(argv[1], "verify_repeat")) {
            FRESULT result = preset_loadProject(0, 0);
            return (result == FR_OK && lxr_get_param(PAR_BPM) == 160 &&
                    seq_getBpm() == 160 && menu_loadedPresetNr[CURRENT_PROJECT] == 0) ? 0 : 4;
        }
        if (!strcmp(argv[1], "verify_project")) {
            FILINFO info;
            FRESULT project_file = f_stat("/proj00/prj.nfo", &info);
            FRESULT loaded = preset_loadProject(0, 0);
            printf("project file=%d project load=%d\n", project_file, loaded);
            int bpm = lxr_get_param(PAR_BPM);
            FILINFO globals_info;
            FRESULT globals_file = f_stat("/proj00/glo.cfg", &globals_info);
            printf("project globals=%d bpm=%d runtime=%u\\n",
                   globals_file, bpm, seq_getBpm());
            int generation = bpm == 145;
            int consistent = check_project_generation(generation) == 0;
            printf("project generation=%s consistent=%d\n",
                   generation ? "new" : "old", consistent);
            return (project_file == FR_OK && info.fsize >= 8 &&
                    globals_file == FR_OK && globals_info.fsize > 0 &&
                    loaded == FR_OK && menu_loadedPresetNr[CURRENT_PROJECT] == 0 &&
                    (bpm == 137 || bpm == 145) && seq_getBpm() == bpm &&
                    consistent) ? 0 : 4;
        }
        return (lxr_get_param(PAR_BPM) == 137 && seq_getBpm() == 137) ? 0 : 4;
    }
    return 2;
}
