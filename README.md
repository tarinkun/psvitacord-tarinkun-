まず私の友人の友人Nさんに感謝いたします(作成を手伝ってもらっています)


# My PS Vita Homebrew

このファイルは、PS Vita向けのcpuHomebrewアプリケーションのです。
VitaSDKとCMakeを使用してビルドされます。

## 機能

- cpuの設定を変更出来るようにします
- GPUの設定を変更出来るようにします
- Xボタンを押すとアプリケーションが終了します。

## ビルド方法

1.  **VitaSDKのセットアップ**:
    事前に[VitaSDK](https://vitasdk.org/)をPCにインストールし、必要な環境変数を設定してください。
    また、`vita2dlib` もビルド済みで利用できるように設定する必要があります。

2.  **リポジトリのクローン**:
    ```bash
    git clone [https://github.com/tarinkun/my-vita-homebrew.git](https://github.com/あなたのユーザー名/my-vita-homebrew.git)
    cd my-vita-homebrew
    ```

3.  **ビルドディレクトリの作成と移動**:
    ```bash
    mkdir build
    cd build
    ```

4.  **CMakeの実行**:
    ```bash
    cmake .. -DCMAKE_TOOLCHAIN_FILE=${VITASDK}/share/vita.toolchain.cmake
    ```

5.  **ビルド**:
    ```bash
    make
    ```
    ビルドが成功すると、`my-vita-homebrew.vpk` ファイルが `build` ディレクトリ内に生成されます。

## PS Vitaへの導入

生成された`.vpk`ファイルをPS Vitaに転送し、VitaShellなどのHomebrewインストーラーでインストールしてください。

## 開発環境

- VitaSDK
- CMake
- `vita2dlib`
