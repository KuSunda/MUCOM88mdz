[WIP]MUCOM88mdz
------------------
## 概要
MUCOM88mdzはNEC PC-8801mkⅡSR以降版のMUCOM88(music.asm(Ver.1.7))をメガドライブ/Genesisに移植することで、
MUCOM88でコンパイルした.mub形式のデータを演奏するサウンドドライバーです。  

- WIP  
音源ドライバのソースコードはリファクタリング出来ておらず、非効率なコード・未使用のコード・間違ったコメントなど散見されます。  
作業時間が取れたら整理したい。  

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
  - DCSGノイズチャンネルは未対応
  - PCM演奏時はBGM/SEが停止

### メガドライブ向けMMLについて
  - VSync駆動  
  Timer-Bの値は参照していません。曲のテンポはクロックと音符の長さで調整となります。
  - G/K(RHYTHM/PCM)チャンネルは非対応  
  SB2のリズム音源・PCMには対応していないのでMMLから削除を推奨します。  
  .mubのサイズは最大0x8000なのでPCMデータをMMLから削除をしてサイズ削減を推奨します。  

  - SSGノイズには対応していない  
  OPNのSSGのノイズはDCSGでは演奏できません。ノイズでの発声を期待したMMLでは想定された音が出力されません。  
  MML修正の参考用にテストモードではwコマンドの設定数が表示されます。
  - SSGの低音部分  
  DCSGの仕様で[o2 a]未満の音は発声できません。
  MML修正の参考用にテストモードで[o2 a]未満の音のリクエスト数をカウントしています。


### 使い方  
- HowToUse.md を参考にしてください。

## リリースノート
  - Ver.0.9  
  WIPで公開  
