[WIP]MUCOM88mdz
------------------
## 使い方

### 演奏データの作成方法
 1).mub
 bintosMub.exeの引数に.mubファイルのリストを与えるとMUCOM88mdz用のファイル一式が作成されます。
 
 2)SE
 cutoutMub.exeの引数に.mubファイルのリストを与えると.mubのSSGチャンネルを切り出してMUCOM88mdz用のファイル一式が作成されます。
 
 3)PCM
 bintosWavに.wavファイルのリストを引数に与えるとMUCOM88mdz用のファイル一式が作成されます。
 サンプリングレートは12KHz/24KHzを選択可能です。デフォルトは12KHzですがZ80のソースコードの[USE_PCM_12K]をマスクすると24Khzです。

### 動作環境
- SGDK(Ver.1.34)のビルド環境を構築してください。Ver.1.34以外でも問題はないと考えています。  
- makefile.gen の有るフォルダで[make -f makefile.gen]を実行してください。
- Z80のドライバーを編集した時は一度[make -f makefile.gen clean]を実行してください。

- SGDK以外の環境について  
68000でプログラムが組める方ならC言語のサンプルを移植できると思います。  
SDGKのassemblerはSjasmなのでこれに特化した記述が有ると思います。  

### ソースコード/ファイルの説明
  - MC68000側  
    - MUCOM88mdz.c/.h  
    68000側のMUCOM88mdzコントロールを行います。
    - MUCOM88mdz_test.c/.h  
    MUCOM88mdzのテストモードです。基本的な使い方の参考にしてください。  
    簡単な実装なので 68000側をアセンブラ変更する事も用意だと思います。

    - MUCOM88mdz_mub.c/MUCOM88mdz_mub.h/MUCOM88mdz_mub??.s
    0x8000毎にアライメントを取った譜面と譜面アドレステーブル・ファイル名テーブルです。  
    bintosMub.exeで自動作成されます。

    - MUCOM88mdz_se.c/MUCOM88mdz_se.h/MUCOM88mdz_se00.s  
    SEの譜面と譜面アドレステーブル・ファイル名テーブルです。  
    cutoutMub.exeで自動作成されます。

    - MUCOM88mdz_pcm.c/MUCOM88mdz_pcm.h/MUCOM88mdz_pcm??.s  
    PCMの波形と波形のアドレステーブルです。  
    bintosWav.exeで自動作成されます。  
  - Z80側  
    - mucom88mdz_drv.s80  
    MUCOM88 Ver.1.1 music.asmをメガドライブに移植したものです。

### .mub/se/pcmの変換方法
mucomフォルダ以下に、MUCOM88mdz向けのデータを作成するサンプルがあります。  
作成したデータをsrcフォルダにコピーしてビルドする事で作成したデータを確認できます。  
.mucファイルのコンパイルには[mucom88.exe]を使っています。PATHの通った場所に配置してください。  

.\mucom  
|   bintosMub.exe : .mubファイルをMUCOM88mdz(BGM)に変換  
|   bintosWav.exe : .wavファイルをMUCOM88mdz(WAV)に変換  
|   cutoutMub.exe : .mubファイルをMUCOM88mdz(SE)に変換  
|   
+---mub  
|       mubfiles.txt  : 変換する.muc(BGM)ファイルのリスト
|       muc2md.bat    : .muc → .mub → MUCOM88mdzに変換
+---pcm  
|       pcm2md.bat  : .wav → MUCOM88mdzに変換
|       pcmlist.txt  : 変換する.wav(PCM)ファイルのリスト
----se  
        se2md.bat   : .muc → .mub → MUCOM88mdzに変換
        se_list.txt  : 変換する.muc(SE)ファイルのリスト

## テストモード
  - 操作方法  
    - Left/Right  
    mub/SE/PCMのリクエスト番号を選択
    - Up/Down  
    mub/SE/PCMのどれを演奏するかの選択
    - A button  
    演奏開始
    - B button  
    演奏停止
    - C button  
    mubのみフェードアウト

  - 標示物について
    - mub  
    曲名は.mubのファイル名です。
    ループ設定のない曲は演奏が終わってもステータスはPLAYのままです。  
    曲終わりは、loopedフラグを確認してください。
    - SE  
    SEの割り込みが終了すると、.mubは次の音符から再開します。なので、全音符がSE割り込まれたら空白期間が長くなる可能性が有ります。  
    - PCM  
    PCM演奏時はZ80側はPCM再生以外の処理は行われません。PCM演奏中に.mub/SEが鳴らしたいときはPCMを止めてください。
    .mub/PCMが同時にリクエストされた場合は.mubを優先しています。(68000側の処理)
    もう一段低いレートが必要でしょうか？

## リリースノート
  - Ver.0.9  
  WIPな状態で公開
