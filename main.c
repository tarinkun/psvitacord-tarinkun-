#include <vita2d.h>           // vita2dlibインクルード
#include <psp2/ctrl.h>        // コントローラー入力
#include <psp2/kernel/processmgr.h> // スリープ、プロセス終了
#include <psp2/power.h>       // CPU/GPUクロック周波数設定用
#include <stdio.h>            // printf、ファイル操作用
#include <string.h>           // memset、strncpy、strrchr用
#include <psp2/io/dirent.h>   // ディレクトリ操作用 (sceIoMkdir)
#include <psp2/io/fcntl.h>    // ファイル操作用 (sceIoOpen/Read/Write/Close)
#include <psp2/kernel/sysclib.h>
#include <psp2/kernel/threadmgr.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544
#define CONFIG_PATH "ux0:data/vita_clock_app/config.conf" // 設定ファイルのパス。アプリケーション名に合わせて変更

// 利用可能なFPSの選択肢
const int available_fps[] = {65, 60, 50, 40, 30};
// 選択肢の数
const int num_fps_options = sizeof(available_fps) / sizeof(available_fps[0]);

// GPU周波数の選択肢を定義
const int gpu_freqs[] = {111, 150, 164, 180, 222}; // MHz
const int num_gpu_freqs = sizeof(gpu_freqs) / sizeof(gpu_freqs[0]);

// --- 設定構造体の定義 ---
typedef struct {
    int cpu_freq;
    int gpu_freq_index; // GPU周波数のインデックスを保存
    int fps_option_index; // FPSオプションのインデックスを保存
} AppConfig;

// --- 設定をファイルからロードする関数 ---
int load_config(AppConfig *config, const char *path) {
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0) {
        printf("Config file not found or failed to open: %s\n", path);
        // ファイルが見つからない場合はデフォルト設定を適用
        memset(config, 0, sizeof(AppConfig)); // 構造体をゼロクリア
        config->cpu_freq = 333; // デフォルトCPU周波数
        config->gpu_freq_index = 2; // デフォルトGPU周波数 (222MHzに対応するインデックス)
        config->fps_option_index = 1; // デフォルトFPS (60FPSに対応するインデックス)
        return -1; // 失敗
    }

    // ファイルから構造体サイズ分を読み込む
    int bytes_read = sceIoRead(fd, config, sizeof(AppConfig));
    sceIoClose(fd);

    if (bytes_read != sizeof(AppConfig)) {
        printf("Error reading config file: bytes_read = %d\n", bytes_read);
        // 読み込みサイズが不正な場合もデフォルト設定を適用
        memset(config, 0, sizeof(AppConfig));
        config->cpu_freq = 333;
        config->gpu_freq_index = 2;
        config->fps_option_index = 1;
        return -1; // 失敗
    }

    printf("Config loaded: CPU=%d, GPU_idx=%d, FPS_idx=%d\n", config->cpu_freq, config->gpu_freq_index, config->fps_option_index);
    return 0; // 成功
}

// --- 設定をファイルに保存する関数 ---
int save_config(const AppConfig *config, const char *path) {
    // ディレクトリが存在しない場合は作成
    char dir_path[256];
    strncpy(dir_path, path, sizeof(dir_path));
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0'; // ファイル名部分を削除してディレクトリパスにする
        sceIoMkdir(dir_path, 0777); // ディレクトリ作成 (エラー無視)
    }

    SceUID fd = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0) {
        printf("Failed to create/open config file for writing: %s, error=0x%08X\n", path, fd);
        return -1; // 失敗
    }

    // 構造体サイズ分をファイルに書き込む
    int bytes_written = sceIoWrite(fd, config, sizeof(AppConfig));
    sceIoClose(fd);

    if (bytes_written != sizeof(AppConfig)) {
        printf("Error writing config file: bytes_written = %d\n", bytes_written);
        return -1; // 失敗
    }

    printf("Config saved: CPU=%d, GPU_idx=%d, FPS_idx=%d\n", config->cpu_freq, config->gpu_freq_index, config->fps_option_index);
    return 0; // 成功
}

// --- アプリケーションのエントリポイント ---
int main(int argc, char *argv[]) {
    // vita2dlibの初期化
    vita2d_init();
    // コントローラーの初期化
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_PLUS); // サンプリングモード設定

    vita2d_font *font = vita2d_load_default_font();

    AppConfig app_config; // 設定構造体のインスタンス
    int result; // 関数の戻り値を格納

    // アプリケーション起動時に設定をロード
    load_config(&app_config, CONFIG_PATH);

    // ロードした設定を現在の周波数とFPSに反映
    scePowerSetCpuClockFrequency(app_config.cpu_freq);
    // ロードしたGPUインデックスが範囲内かチェック
    if (app_config.gpu_freq_index < 0 || app_config.gpu_freq_index >= num_gpu_freqs) {
        app_config.gpu_freq_index = num_gpu_freqs - 1; // 無効な場合はデフォルト（最高周波数）に設定
    }
    scePowerSetGpuClockFrequency(gpu_freqs[app_config.gpu_freq_index]);
    // ロードしたFPSインデックスが範囲内かチェック
    if (app_config.fps_option_index < 0 || app_config.fps_option_index >= num_fps_options) {
        app_config.fps_option_index = 1; // 無効な場合はデフォルト（60FPS）に設定
    }

    // 実際に設定された周波数を取得し直す
    int current_cpu_freq = scePowerGetCpuClockFrequency();
    int current_gpu_freq = scePowerGetGpuClockFrequency();
    int current_target_fps = available_fps[app_config.fps_option_index];
    SceUInt64 current_frame_time_us = 1000000 / current_target_fps;

    SceUInt64 last_frame_time_us = sceKernelGetSystemTimeWide();
    SceUInt64 current_time_us = 0;
    SceUInt64 elapsed_time_us = 0;
    SceUInt64 sleep_time_us = 0;

    // メインループ
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break; // ループを抜けてアプリケーション終了処理へ
        }

        // --- CPU周波数変更のロジック (R1/L1で変更) ---
        if (pad.buttons & SCE_CTRL_R1) {
            int new_freq = 444; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                app_config.cpu_freq = new_freq; // 設定を更新
                printf("CPU frequency set to %d MHz\n", new_freq);
                save_config(&app_config, CONFIG_PATH); // 変更を保存
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }

        if (pad.buttons & SCE_CTRL_L1) {
            int new_freq = 333; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                app_config.cpu_freq = new_freq; // 設定を更新
                printf("CPU frequency set to %d MHz\n", new_freq);
                save_config(&app_config, CONFIG_PATH); // 変更を保存
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }
        // --- CPU周波数変更ロジックここまで ---

        // --- GPU周波数変更のロジック (方向キー左右で変更) ---
        if (pad.buttons & SCE_CTRL_RIGHT) {
            app_config.gpu_freq_index++; // 設定構造体内のインデックスを直接操作
            if (app_config.gpu_freq_index >= num_gpu_freqs) {
                app_config.gpu_freq_index = num_gpu_freqs - 1; // 最大値でクランプ
            }
            int new_freq = gpu_freqs[app_config.gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
                save_config(&app_config, CONFIG_PATH); // 変更を保存
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }

        if (pad.buttons & SCE_CTRL_LEFT) {
            app_config.gpu_freq_index--; // 設定構造体内のインデックスを直接操作
            if (app_config.gpu_freq_index < 0) {
                app_config.gpu_freq_index = 0; // 最小値でクランプ
            }
            int new_freq = gpu_freqs[app_config.gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
                save_config(&app_config, CONFIG_PATH); // 変更を保存
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }
        // --- GPU周波数変更ロジックここまで ---

        // --- FPS変更のロジック (方向キー上下で変更) ---
        if (pad.buttons & SCE_CTRL_UP) {
            app_config.fps_option_index--;
            if (app_config.fps_option_index < 0) {
                app_config.fps_option_index = num_fps_options - 1; // ラップアラウンド
            }
            current_target_fps = available_fps[app_config.fps_option_index];
            current_frame_time_us = 1000000 / current_target_fps;
            printf("Target FPS set to %d\n", current_target_fps);
            save_config(&app_config, CONFIG_PATH); // 変更を保存
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }
        if (pad.buttons & SCE_CTRL_DOWN) {
            app_config.fps_option_index++;
            if (app_config.fps_option_index >= num_fps_options) {
                app_config.fps_option_index = 0; // ラップアラウンド
            }
            current_target_fps = available_fps[app_config.fps_option_index];
            current_frame_time_us = 1000000 / current_target_fps;
            printf("Target FPS set to %d\n", current_target_fps);
            save_config(&app_config, CONFIG_PATH); // 変更を保存
            sceKernelDelayThread(150 * 1000); // 短いディレイ
        }
        // --- FPS変更ロジックここまで ---

        // 描画開始
        vita2d_start_drawing();
        vita2d_clear_screen(); // 画面を黒でクリア

        // 現在のCPU周波数を画面に表示
        char cpu_freq_text[64];
        sprintf(cpu_freq_text, "CPU: %d MHz (R1/L1)", current_cpu_freq);
        vita2d_draw_text(font, 10, 50, RGBA8(255, 255, 255, 255), 2.0f, cpu_freq_text);

        // 現在のGPU周波数を画面に表示
        char gpu_freq_text[64];
        sprintf(gpu_freq_text, "GPU: %d MHz (Left/Right D-Pad)", current_gpu_freq);
        vita2d_draw_text(font, 10, 100, RGBA8(255, 255, 255, 255), 2.0f, gpu_freq_text);

        // 現在のFPS設定を表示
        char fps_text[64];
        sprintf(fps_text, "Target FPS: %d (UP/DOWN D-Pad)", current_target_fps);
        vita2d_draw_text(font, 10, 150, RGBA8(255, 255, 0, 255), 2.0f, fps_text);
        
        // 操作説明
        vita2d_draw_text(font, 10, 250, RGBA8(255, 255, 255, 255), 1.0f, "Press R1/L1 for CPU freq");
        vita2d_draw_text(font, 10, 280, RGBA8(255, 255, 255, 255), 1.0f, "Press D-Pad Left/Right for GPU freq");
        vita2d_draw_text(font, 10, 310, RGBA8(255, 255, 255, 255), 1.0f, "Press D-Pad Up/Down for Target FPS");
        vita2d_draw_text(font, 10, 340, RGBA8(255, 0, 0, 255), 1.0f, "Press X to Exit");
            
        // 描画終了
        vita2d_end_drawing();
        vita2d_swap_buffers(); // 描画された内容を画面に表示
        vita2d_sync_wait_for_vblank_start(); // 垂直同期を待つ

        // FPS調整機能 (フレームレートの安定化)
        current_time_us = sceKernelGetSystemTimeWide();
        elapsed_time_us = current_time_us - last_frame_time_us;

        if (elapsed_time_us < current_frame_time_us) {
            sleep_time_us = current_frame_time_us - elapsed_time_us;
            sceKernelDelayThread(sleep_time_us);
        }
        last_frame_time_us = sceKernelGetSystemTimeWide();
    }

    // アプリケーション終了前のクリーンアップ
    vita2d_free_font(font);
    vita2d_fini();
    sceKernelExitProcess(0); // プロセスを終了
    return 0;
}
