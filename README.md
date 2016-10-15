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

## 使いかた
`simulator`に機械語ファイルをわたして起動します。

画面の上半分にはレジスタの値が表示されます。
下半分には命令列が書かれています。'>'印がついているのが、次に実行する命令です。
最下部にはコマンドが入力できます。
* r -- halt命令まで進めます。
* s -- 命令をひとつ実行します。
* p -- ひとつ前の状態に戻ります。
* q -- 終了します。
* h -- ヘルプを表示します。
