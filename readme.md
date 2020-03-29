MUCOM88mdz
------------------
## 概要
MUCOM88mdzはNEC PC-8801mkⅡSR以降版のMUCOM88(music.asm(Ver.1.7))をメガドライブ/Genesisに移植することで、
MUCOM88でコンパイルした.mub形式のデータを演奏するサウンドドライバーです。  

- Ver.1.0リリース  
DCSGのノイズに対応した事によりMucom88コンパイラ/メガドライブ(Z80)で演奏するコンセプトは一通り実装完了したと思います。
基本機能は実装できたのでVer.1.0とします。
追加機能や不具合修正はのんびり進めます。

## ライセンスについて
MUCOM88mdzはMUCOM88から移植・派生した音源ドライバーとそれの制御プログラムです。  
ライセンスはオリジナルのMUCOM88(License: CC BY-NC-SA 4.0)に準拠します。  
詳細は本家のサイトをご確認ください。

[OPEN MUCOM PROJECT (株式会社エインシャント様)](https://www.ancient.co.jp/~MUCOM88/)  
 MUCOM88公式サイト。オリジナルのMUCOM88はこちらのサイトで配布されています。

[OPEN MUCOM88 Wiki (ONION software/おにたま様)](https://github.com/onitama/MUCOM88/wiki)  
 OPEN MUCOM88 公式Wikiサイト。MUCOM88の各種情報はこちらのサイトをご参照下さい。

## MUCOM88mdzについて  
MUCOM88mdzの**mdz**は、md:MegaDrive / z:Z80版ドライバーを意味しています。  
移植の目的はMUCOM88に興味が有った・DCSGでSEを演奏させたい・68000の負荷を軽くしてシューティングを作りやすくしたかったとなります。  

メガドライブ開発環境はSGDK(Ver.1.34)で作成されています。  
https://github.com/Stephane-D/SGDK

### MUCOM88mdzの仕様
  - BGM(.mub)をFM6チャンネル/DCSG3チャンネルで演奏
  - .mubファイルは、1ファイルにつき1曲の仕様
  - DCSG3チャンネルにSEを割り込んで演奏
  - BGM/SEと排他的にPCMの演奏
  - BGMは1曲あたり最大0x8000
  - SEは.mubファイルからDCSGの部分を切り出して使用
  - SEは譜面とアドレステーブル含めて最大0x8000
  - PCMファイルは1音あたり最大0x8000
  - Z80で音源を駆動
  - VSyncのタイミングでの駆動(Timer-Bの値は参照していません)
  - ループ […]n コマンドのネストは4回までに制限されています。

### MUCOM88mdzで出来ないこと
  - BGMでリズム音源・PCMの再生は不可能
  - PCM演奏時はBGM/SEが停止

### メガドライブ向けMMLについて
  - VSync駆動  
  Timer-Bの値は参照していません。曲のテンポはクロックと音符の長さで調整となります。
  - G/K(RHYTHM/PCM)チャンネルは非対応  
  SB2のリズム音源・PCMには対応していないのでMMLから削除を推奨します。  
  .mubのサイズは最大0x8000なのでPCMデータをMMLから削除をしてサイズ削減を推奨します。  

  - SSGの低音部分  
  DCSGの仕様で[o2 a]未満の音は発声できません。
  MML修正の参考用にテストモードで[o2 a]未満の音のリクエスト数を[OCT:00]の部分でカウントしている。


### 使い方  
- HowToUse.md を参考にしてください。

## リリースノート
  - Ver.1.0  
    - Noise対応(FチャンネルでP/w(PC88互換ノイズ)コマンドに対応)  
  P		[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ)  
  w		[SSG] ノイズ周波数 (0～31)  
  mucom88mdz_drv.s80に、wパラメータ変換テーブルTP5BitToDcsg12BItTblが有ります。SSGからDCSGへのノイズ周波数の調整が可能  
    - CUE  
  テストモードにZボタンでMubの演奏を4倍速にするデバッグ機能を追加  
    - サンプル  
    Mub/SEにノイズのサンプルを追加
    - Z80のコードのリファクタリング  
    コメントの整理  
    コードの整理効率化  
    Rhythm/PCM(SB2)向けコードの削除・マスクを行いコード量・実行速度の最適化  

  - Ver.0.9  
  WIPで公開  
