# Set up an environment

## install Dependencies

```bash
sudo apt update
sudo apt install build-essential curl file git
```

## install Homebrew

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

## set up path

```bash
echo 'eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"' >> ~/.bashrc
eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"
```

## check if Homebrew works

```bash
brew --version
```

## Install relevant packages

```bash
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavfilter-dev libavdevice-dev
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

GLU development library

```bash
sudo apt-get update
sudo apt-get install libglu1-mesa-dev
```

# build and run

mkdir build
cd build
cmake ..
make

## generate colors

```cpp
unsigned char *data = new unsigned char[100 * 100 * 3];
for (int y = 0; y < 100; ++y)
{
    for (int x = 0; x < 100; ++x)
    {
        data[y * 100 * 3 + x * 3] = 0xff;     // red
        data[y * 100 * 3 + x * 3 + 1] = 0x00; // green
        data[y * 100 * 3 + x * 3 + 2] = 0x00; // blue
    }
}
```


## FFmpeg

https://www.ffmpeg.org/


## GLFW 
### ライブラリ

```cpp
#include <GLFW/glfw3.h>
```

- ウィンドウの作成と管理
OpenGLで描画するためのウィンドウを作成・管理することができる。

- コンテキスト管理
OpenGLコンテキストを作成して使用するための管理を行う。

- 入力処理
キーボードやマウスの入力処理が容易に行える。

- マルチモニターのサポート
複数のモニターに対応しており、モニター情報の取得や特定のモニターでのウィンドウの表示などができる。

### 各メソッド・構造体の解説
#### GLFWwindow
```cpp
GLFWwindow *window;
```
GLFWwindowは、GLFWライブラリ内で定義されている構造体型であり、
ウィンドウやOpenGLコンテキストに関する情報を保持している。
子の構造体は直接操作はできないが、ウィンドウの作成、管理、操作を行うためにGLFWの関数に渡される。

windowは、GLFWwindow構造体を指すポインタである。このポインタは、
glfwCreateWindow関数を呼び出してウィンドウを作成したときに、作成されたウィンドウの情報を受け取る。

#### glfwCreateWindow (method)
```cpp
GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
```

この関数は、指定されたサイズ、タイトル、および設定に基づいて新しいウィンドウを作成し、そのウィンドウを指すポインタを返す。

パラメータの説明
1. int width, int height
作成
するウィンドウの幅と高さを指定する。

2. const char* title
ウィンドウのタイトルを指す。

3. GLFWmonitor* monitor
フルスクリーンウィンドウを作成する場合に使用するモニターを指定する。
NULLを渡すことで、ウィンドウモード（通常のウィンドウ）として作成される。

4. GLFWwindow *share
OpenGLコンテキストを共有するために使う。他のウィンドウとリソース（例えばテクスチャやバッファ）を共有したい場合に使う。
NULLを渡すことで、コンテキストを共有しない。

返り値
windowは、GLFWwindow*型のポインタで、作成されたウィンドウを指す。
ウィンドウの作成に成功すれば有効なポインタが返されるが、失敗すればNULLが返される。

#### glfwMMakeContextCurrent(window) (method)
OpenGLコンテキストを現在のスレッドに関連付けるためのGLFW関数である。

OpenGLコンテキストとは？
OpenGLコンテキストは、OpenGLでの描画に必要な状態やリソース（バッファ、シェーダーなど）を管理するオブジェクトである。
描画を行うために、必ず1つのコンテキストが現在のスレッドに関連付けられている必要がある。

この関数は、指定したウィンドウのOpenGLコンテキストを現在のスレッドに関連付ける。
これにより、以降のOpenGLコマンドはこのコンテキストに対して実行され、ウィンドウに描画することが可能になる。

#### GLuint
GLuintは、OpenGLで使われる符号なしの整数の型で、主にリソース（テクスチャ、バッファ、シェーダーなど）のIDを格納する。

#### glGenTextures(GLsizei n, GLuint* textures)
glGenTextures(GLsizei n, GLuint* textures)は、n個のテクスチャオブジェクトを生成している。

実際のテクスチャオブジェクトが生成されると、OpenGLは内部的にそのIDを管理し、
今後の操作でそのIDを通してテクスチャにアクセスする。

#### glBindTexture(GLenum target, GLuint texture)
glBindTexture(GLenum target, GLuint texture)は、指定されたtargetに対して、テクスチャオブジェクトをバインドする。

#### GL_TEXTURE_2D
GL_TEXTURE_2Dは、2Dテクスチャターゲットを指定する。これは2D画像を扱うテクスチャを意味する。