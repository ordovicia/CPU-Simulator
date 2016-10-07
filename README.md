Felis Simulator
===============

2016年度CPU実験B班の1stアーキテクチャ[Felis](https://github.com/wafrelka/felis)用シミュレータ。

## ビルド方法
clone後初回ビルド時

```shell
$ mkdir build; cd $_
$ cmake ..
$ make gen_initInst
$ cmake ..
$ make
```

二回目以降

```shell
$ cd build
$ cmake ..  # 必要なら
$ make
```
