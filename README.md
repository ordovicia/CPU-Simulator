Felis Simulator
===============

2016年度CPU実験B班の1stアーキテクチャ[Felis](https://github.com/wafrelka/felis)用シミュレータ。

## ビルド方法
```shell
$ mkdir build; cd $_
$ cmake ..
$ make
```

リンカエラーを起したときは、もう一回`cmake ..`からやり直せば通ることがあります。

## 使いかた
`simulator`は次のように、`-f`オプションに機械語ファイルを渡して実行します。

```shell
$ ./simulator -f test.bin
```

他にもいくつかオプションがあります。

* `-r` -- `halt`命令まですすめ、レジスタの値や統計情報を出力して終了します。
* `-m` -- 統計情報に、メモリの状態を含めます。

`-r`オプションで、出力されるものは

* `call_cnt.log`に、動的命令数と、PCごとの呼ばれた回数。
* `instruction.log`に、命令ごとの呼ばれた回数。
* `register.log`に、最終的なレジスタの状態。
* `memory.log`に、最終的なメモリの状態。`-m`オプションが指定されているときのみ。

となっています。

`-r`オプションを指定しない場合、インタラクティブに実行できます。
画面の上半分にはレジスタの値が表示されます。
下半分には命令列と逆アセンブル結果が書かれています。反転しているのが、次に実行する命令です。
逆アセンブル結果の形式は、

* Type R -- mnemonic, rs, rt, rd, shamt
* Type I -- mnemonic, rs, rt, (immediateが負なら符号拡張したものを符号付き、正なら符号なし)
* Type J -- mnemonic, addr

となっています。

最下部にはコマンドが入力できます。

* run|r -- halt命令まで進めます。
* reset -- 初期状態にリセットします。
* (break|b) [int] -- 指定したプログラムカウンタにbreakpointをはります。
* pb -- breakpointを表示します。
* db [int] -- 指定したbreakpointを削除します。
* pm [int] -- 指定したインデックスのメモリの状態を表示します。
* step|s -- 命令をひとつ実行します。
* prev|p -- ひとつ前の状態に戻ります。
* quit|q -- 終了します。
* help|h -- ヘルプを表示します。
