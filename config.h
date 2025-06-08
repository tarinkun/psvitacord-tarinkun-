#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#define CONFIG_PATH "ux0:data/MyVitaFreqApp/config.conf" // 設定ファイルのパス

// 設定を保存するための構造体
typedef struct {
    int cpu_freq;
    int gpu_freq_index; // GPU周波数のインデックスを保存;
} AppConfig;

#endif // _APP_CONFIG_H_
