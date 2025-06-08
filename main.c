#include <vita2d.h>           // vita2dlibインクルード
#include <psp2/ctrl.h>        // コントローラー入力
#include <psp2/kernel/processmgr.h> // スリープ、プロセス終了
#include <psp2/power.h>       // CPU/GPUクロック周波数設定用
#include <stdio.h>            // printf用
#include <stdio.h>    // ファイル操作とprintf用
#include <string.h>   // memset用
#include <psp2/io/dirent.h> // ディレクトリ操作用 (SceIoMkdir)
#include <psp2/io/fcntl.h>  // ファイル操作用 (open/read/write/close)


#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

// アプリケーションのエントリポイント
int main(int argc, char *argv[]) {
    // vita2dlibの初期化
    vita2d_init();
    // コントローラーの初期化
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    
    // 現在のCPU/GPU周波数を取得
    int current_cpu_freq = scePowerGetCpuClockFrequency();
    int current_gpu_freq = scePowerGetGpuClockFrequency();
    int result; // 関数の戻り値を格納

    // GPU周波数の選択肢を定義（一般的な値）
    // 好みに応じて調整してください
    const int gpu_freqs[] = {111, 150, 164, 180, 222}; // MHz
    const int num_gpu_freqs = sizeof(gpu_freqs) / sizeof(gpu_freqs[0]);
    int gpu_freq_index = 0; // 現在選択されている周波数のインデックス

    // 現在のGPU周波数がリストにあれば、初期インデックスを設定
    for (int i = 0; i < num_gpu_freqs; i++) {
        if (gpu_freqs[i] == current_gpu_freq) {
            gpu_freq_index = i;
            break;
        }
    }

    // メインループ
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break;
        }

        // --- CPU周波数変更のロジック (R1/L1で変更) ---
        // R1ボタンが押されたらCPU周波数を上げる
        if (pad.buttons & SCE_CTRL_R1) {
            int new_freq = 444; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                printf("CPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            // 連打防止: ボタンを離すまで待機
            while (pad.buttons & SCE_CTRL_R1) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }

        // L1ボタンが押されたらCPU周波数を下げる
        if (pad.buttons & SCE_CTRL_L1) {
            int new_freq = 333; // MHz (元のコードは301でしたが、より一般的な333MHzに修正)
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                printf("CPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            // 連打防止: ボタンを離すまで待機
            while (pad.buttons & SCE_CTRL_L1) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }
        // --- CPU周波数変更ロジックここまで ---

        // --- GPU周波数変更のロジック (方向キー左右で変更) ---
        if (pad.buttons & SCE_CTRL_RIGHT) {
            gpu_freq_index++;
            if (gpu_freq_index >= num_gpu_freqs) {
                gpu_freq_index = num_gpu_freqs - 1; // 最大値でクランプ
            }
            int new_freq = gpu_freqs[gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            // 連打防止: ボタンを離すまで待機
            while (pad.buttons & SCE_CTRL_RIGHT) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }

        if (pad.buttons & SCE_CTRL_LEFT) {
            gpu_freq_index--;
            if (gpu_freq_index < 0) {
                gpu_freq_index = 0; // 最小値でクランプ
            }
            int new_freq = gpu_freqs[gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            // 連打防止: ボタンを離すまで待機
            while (pad.buttons & SCE_CTRL_LEFT) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }
        // --- GPU周波数変更ロジックここまで ---


        // 描画開始
        vita2d_start_drawing();
        vita2d_clear_screen(); // 画面を黒でクリア

        // 現在のCPU周波数を画面に表示
        char cpu_freq_text[64];
        sprintf(cpu_freq_text, "CPU: %d MHz (R1/L1)", current_cpu_freq);
        vita2d_draw_string(10, 50, RGBA8(255, 255, 255, 255), 2.0f, cpu_freq_text);

        // 現在のGPU周波数を画面に表示
        char gpu_freq_text[64];
        sprintf(gpu_freq_text, "GPU: %d MHz (Left/Right D-Pad)", current_gpu_freq);
        vita2d_draw_string(10, 100, RGBA8(255, 255, 255, 255), 2.0f, gpu_freq_text);
        
        // 操作説明
        vita2d_draw_string(10, 200, RGBA8(255, 255, 0, 255), 1.0f, "Press R1/L1 for CPU, D-Pad Left/Right for GPU");
        vita2d_draw_string(10, 230, RGBA8(255, 0, 0, 255), 1.0f, "Press X to Exit");
            
        // 描画終了
        vita2d_end_drawing();
        vita2d_swap_buffers(); // 描画された内容を画面に表示

        // 短いスリープでCPU使用率を下げる（VITAのフレームレートに合わせる）
        sceKernelDelayThread(1000 * 1000 / 60); // 約16ms待機で60FPSを目指す
    }

    // アプリケーション終了前のクリーンアップ
    vita2d_fini();
    sceKernelExitProcess(0); // プロセスを終了
    return 0;
}
