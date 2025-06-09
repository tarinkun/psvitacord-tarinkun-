#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <psp2/types.h>

// 設定構造体
typedef struct {
    int target_fps;
} AppConfig;

// 設定ファイルへのパス
#define CONFIG_FILE_PATH "ux0:/data/vita_select/config.ini"

// 設定をロードする関数
int load_config(AppConfig *config);

// 設定を保存する関数
int save_config(const AppConfig *config);

#endif // CONFIG_MANAGER_H
