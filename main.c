#include <vita2d.h> // vita2dlibインクルード
#include <psp2/ctrl.h> // コントローラー入力
#include <psp2/kernel/processmgr.h> // スリープ

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

// アプリケーションのエントリポイント
int main(int argc, char *argv[]) {
    // vita2dlibの初期化
    vita2d_init();
    // コントローラーの初期化
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    
    int current_cpu_freq = scePowerGetCpuClockFrequency(); // 現在のCPU周波数を取得
    int result; // 関数の戻り値を格納

    // メインループ
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break;
        }

        // --- CPU周波数変更のロジック ---

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
            //連打防止
            while (pad.buttons & SCE_CTRL_R1) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }

        // L1ボタンが押されたらCPU周波数を下げる
        if (pad.buttons & SCE_CTRL_L1) {
            int new_freq = 301; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                printf("CPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            // 連打防止
            while (pad.buttons & SCE_CTRL_L1) {
                sceCtrlReadBufferPositive(0, &pad, 1);
                sceKernelDelayThread(1000 * 1000 / 60);
            }
        }
        // --- CPU周波数変更ロジックここまで ---

        // 描画開始
        vita2d_start_drawing();
        vita2d_clear_screen();

        // 現在のCPU周波数を画面に表示
        char freq_text[64];
        sprintf(freq_text, "Current CPU: %d MHz", current_cpu_freq);
        vita2d_draw_string(10, 50, RGBA8(255, 255, 255, 255), 2.0f, freq_text);
        vita2d_draw_string(10, 100, RGBA8(255, 255, 0, 255), 1.0f, "Press R1 to Increase, L1 to Decrease");
        vita2d_draw_string(10, 150, RGBA8(255, 0, 0, 255), 1.0f, "Press X to Exit");

int main(int argc, char *argv[]) {
    vita2d_init();
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    int current_cpu_freq = scePowerGetCpuClockFrequency();
    int current_gpu_freq = scePowerGetGpuClockFrequency();
    int result;

    // GPU周波数の選択肢を定義（一般的な値）
    // 好みに応じて調整してください
    const int gpu_freqs[] = {111,150,164,180, 222}; // MHz
    const int num_gpu_freqs = sizeof(gpu_freqs) / sizeof(gpu_freqs[0]);
    int gpu_freq_index = 0; // 現在選択されている周波数のインデックス

    // 現在のGPU周波数がリストにあれば、初期インデックスを設定
    for (int i = 0; i < num_gpu_freqs; i++) {
        if (gpu_freqs[i] == current_gpu_freq) {
            gpu_freq_index = i;
            break;
        }
    }


    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break;
        }

        // --- CPU周波数変更のロジック (R1/L1で変更) ---
        if (pad.buttons & SCE_CTRL_R1) {
            int new_freq = 444; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                printf("CPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            while (pad.buttons & SCE_CTRL_R1) { sceCtrlReadBufferPositive(0, &pad, 1); sceKernelDelayThread(1000 * 1000 / 60); }
        }

        if (pad.buttons & SCE_CTRL_L1) {
            int new_freq = 333; // MHz
            result = scePowerSetCpuClockFrequency(new_freq);
            if (result == 0) {
                current_cpu_freq = new_freq;
                printf("CPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set CPU frequency: 0x%08X\n", result);
            }
            while (pad.buttons & SCE_CTRL_L1) { sceCtrlReadBufferPositive(0, &pad, 1); sceKernelDelayThread(1000 * 1000 / 60); }
        }
        // --- CPU周波数変更ロジックここまで ---

        // --- GPU周波数変更のロジック (方向キー左右で変更) ---
        if (pad.buttons & SCE_CTRL_RIGHT) {
            gpu_freq_index++;
            if (gpu_freq_index >= num_gpu_freqs) {
                gpu_freq_index = num_gpu_freqs - 1; // 最大値クランプ
            }
            int new_freq = gpu_freqs[gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            // ボタンを離すまで待つ
            while (pad.buttons & SCE_CTRL_RIGHT) { sceCtrlReadBufferPositive(0, &pad, 1); sceKernelDelayThread(1000 * 1000 / 60); }
        }

        if (pad.buttons & SCE_CTRL_LEFT) {
            gpu_freq_index--;
            if (gpu_freq_index < 0) {
                gpu_freq_index = 0; // 最小値クランプ
            }
            int new_freq = gpu_freqs[gpu_freq_index];
            result = scePowerSetGpuClockFrequency(new_freq);
            if (result == 0) {
                current_gpu_freq = new_freq;
                printf("GPU frequency set to %d MHz\n", new_freq);
            } else {
                printf("Failed to set GPU frequency: 0x%08X\n", result);
            }
            // ボタンを離すまで待つ
            while (pad.buttons & SCE_CTRL_LEFT) { sceCtrlReadBufferPositive(0, &pad, 1); sceKernelDelayThread(1000 * 1000 / 60); }
        }
        // --- GPU周波数変更ロジックここまで ---


        // 描画開始
        vita2d_start_drawing();
        vita2d_clear_screen();

        // 現在のCPU周波数を画面に表示
        char cpu_freq_text[64];
        sprintf(cpu_freq_text, "CPU: %d MHz (R1/L1)", current_cpu_freq);
        vita2d_draw_string(10, 50, RGBA8(255, 255, 255, 255), 2.0f, cpu_freq_text);

        
        // 現在のGPU周波数を画面に表示
        char gpu_freq_text[64];
        sprintf(gpu_freq_text, "GPU: %d MHz (Left/Right D-Pad)", current_gpu_freq);
        vita2d_draw_string(10, 100, RGBA8(255, 255, 255, 255), 2.0f, gpu_freq_text);

        vita2d_draw_string(10, 200, RGBA8(255, 0, 0, 255), 1.0f, "Press X to Exit")
            
        // 描画終了
        vita2d_end_drawing();
        vita2d_swap_buffers();

        
        sceKernelDelayThread(1000 * 1000 / 61);


        sceKernelDelayThread(1000 * 1000 / 60); // 1/60秒待機
    }

    // アプリケーション終了前のクリーンアップ
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}

        
