#include "config_manager.h"
#include <stdio.h>
#include <string.h>
#include <psp2/io/fcntl.h> // ファイル操作のために必要
#include <psp2/io/dirent.h> // ディレクトリ操作のために必要

// デフォルト設定
static AppConfig default_config = {
    .target_fps = 60 // デフォルトのFPS
};

// 設定をロードする関数
int load_config(AppConfig *config) {
    FILE *fp;
    char line[256];
    char key[64];
    char value[64];

    // デフォルト設定をロード
    *config = default_config;

    fp = fopen(CONFIG_FILE_PATH, "r");
    if (!fp) {
        // ファイルが存在しない場合は、デフォルト設定で続行
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        // コメントと改行をスキップ
        if (line[0] == '#' || line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // キーと値のペアをパース (例: key=value)
        if (sscanf(line, "%63[^=]=%63[^\n\r]", key, value) == 2) {
            if (strcmp(key, "TargetFPS") == 0) {
                config->target_fps = atoi(value);
            }
            // 他の設定項目があればここに追加
        }
    }

    fclose(fp);
    return 0;
}

// 設定を保存する関数
int save_config(const AppConfig *config) {
    FILE *fp;
    char dir_path[256];
    const char *last_slash;

    // ディレクトリパスを抽出
    last_slash = strrchr(CONFIG_FILE_PATH, '/');
    if (last_slash) {
        strncpy(dir_path, CONFIG_FILE_PATH, last_slash - CONFIG_FILE_PATH);
        dir_path[last_slash - CONFIG_FILE_PATH] = '\0';
        // ディレクトリが存在しない場合は作成
        sceIoMkdir(dir_path, 0777); // 権限を0777に設定
    }

    fp = fopen(CONFIG_FILE_PATH, "w");
    if (!fp) {
        return -1; // ファイルを開けない場合はエラー
    }

    fprintf(fp, "TargetFPS=%d\n", config->target_fps);
    // 他の設定項目があればここに追加

    fclose(fp);
    return 0;
}
