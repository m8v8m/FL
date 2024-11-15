环境:win10 x64
MingW64
VScode
所有代码未曾在VS Studio运行,均使用MingW编译 
  1.#pragma comment ( lib,"xxx.lib" )  需要编译时添加命令:-lxxx.lib
  2.编译出来的exe提示缺少dll，需要编译时添加命令：-static（编译出来的exe体积会大很多）
