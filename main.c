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

    // メインループ
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break;
        }

        // 描画開始
        vita2d_start_drawing();
        // 背景を黒でクリア
        vita2d_clear_screen();

        // 文字列「Hello World!」を描画
        // vita2d_font_draw_textf(font_ptr, x, y, color, scale, "フォーマット文字列", ...)
        // フォントがロードされていないので、デフォルトのフォントを使う
        // デフォルトフォントで日本語が表示されるかは環境による
        vita2d_draw_string(100, 100, RGBA8(255, 255, 255, 255), 2.0f, "tarinkun vitaset start"); // 白で表示

        // 描画終了
        vita2d_end_drawing();
        // 画面をフリップ（描画された内容を表示）
        vita2d_swap_buffers();

        //CPU使用率を下げる
        sceKernelDelayThread(1000 * 1000 / 60);
    }

    // vita2dlibの終了
    vita2d_fini();
    // アプリケーション終了
    sceKernelExitProcess(0);
    return 0;
}
