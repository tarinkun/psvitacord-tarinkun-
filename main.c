
#include <vitasdk.h> // VitaSDKをインクルード

// アプリケーションのエントリポイント
int main(int argc, char *argv[]) {
    // スクリーンを初期化
    sceDisplaySetFrameBuf(
        &(SceDisplayFrameBuf){
            .size = sizeof(SceDisplayFrameBuf),
            .base = 0x00000000,
            .pitch = 960,
            .pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8,
            .common = SCE_DISPLAY_SETBUF_CONT_WIDTH | SCE_DISPLAY_SETBUF_CONT_HEIGHT
        }, SCE_DISPLAY_SETBUF_NEXTFRAME);
    SceAppUtilInitParam init_param;
    SceAppUtilBootParam boot_param;
    memset(&init_param, 0, sizeof(SceAppUtilInitParam));
    memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
    sceAppUtilInit(&init_param, &boot_param);

    
    printf("tarinkun start\n");

  
    //
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(0, &pad, 1);

        // Xボタンが押されたら終了
        if (pad.buttons & SCE_CTRL_CROSS) {
            break;
        }

        // CPU使用率を下げる
        sceKernelDelayThread(1000 * 1000 / 60); // 
    }

    // クリーンアップ
    sceAppUtilShutdown();

    return 0; // 終了
}
