[WIP]MUCOM88mdz
------------------
## 使い方
### ビルド環境
- SGDK(Ver.1.34)で動作確認を行っています。  
- makefile.gen の有るフォルダで[make -f makefile.gen]を実行してください。  
[.\out\\rom.bin]をエミュレーターなどで起動するとテストモードが起動します。  
- Z80のドライバーを編集した時は一度[make -f makefile.gen clean]を実行してください。  

- SGDK以外の環境について  
Z80のドライバーはSGDK(sjasm)でアセンブルしています。これに特化した記述が有ると思います。  
68000でプログラムが組める方ならC言語のコードを参考に移植は容易だと思います。  


### 演奏データの作成方法
- BGM  
BGMは1曲(mubファイル)あたり最大サイズは0x8000迄です。PCMファイル指定/G/Kチャンネルは削除をお勧めします。  
[bintosMub.exe]でmubファイルをMUCOM88mdz向けに変換します。  
  - 変換方法  
  ex) bintosMub.exe mublist.txt  
  mublist.txtに変換したいmubファイルを並べて実行すると、C言語のソース・ヘッダー・アセンブラ形式のデータが作成されます。  
  ex)MUCOM88mdz_mub.c/MUCOM88mdz_mub.h/MUCOM88mdz_mub??.s
  
- SE  
SEは譜面データ+オフセットテーブルを合わせて最大サイズは0x8000迄です。  
[cutoutMub.exe]mubファイルからD/E/Fチャンネルを切り出してMUCOM88mdz向けに変換します。  
  - 変換方法  
  \>cutoutMub.exe mublist.txt  
  mublist.txtに変換したいmubファイルを並べて実行すると、C言語のソース・ヘッダー・アセンブラ形式のデータが作成されます。  
  ex) MUCOM88mdz_se.c/MUCOM88mdz_se.h/MUCOM88mdz_se00.s  

 
- PCM  
PCMは1音あたり最大サイズは0x8000迄です。(最大12KHz:約2.66秒 24KHz:約1.33秒)  
[bintosWav]でWAVファイル(12KHzか24KHzモノラルフォーマット)をMUCOM88mdz向けに変換しますがエラーチェックはしていません。  
サンプリングレートはデフォルト12KHzですがZ80のソースコード内の定義[USE_PCM_12K]をマスクすることで24KHzに変更することで切り替え可能です。  
  - 変換方法  
  \>bintosWav.exe wavlist.txt  
  wavlist.txtに変換したいwavファイルを並べて実行すると、C言語のソース・ヘッダー・アセンブラ形式のデータが作成されます。  
  ex) MUCOM88mdz_pcm.c/MUCOM88mdz_pcm.h/MUCOM88mdz_pcm??.s  


### ソースコード/ファイルの説明
  - MC68000側  
    - MUCOM88mdz.c/.h  
    68000側のMUCOM88mdzコントロールを行います。

    - MUCOM88mdz_test.c/.h  
    MUCOM88mdzのテストモードです。ゲームに組み込むときの参考にしてください。  
    68000側をアセンブラで記述することも容易だと思います。(変換ツールが未対応ですが、、、、)  

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

### .mub/se/pcmの変換例
mucomフォルダ以下に、MUCOM88mdz向けのデータを作成するサンプルがあります。  
作成したデータをsrcフォルダにコピーしてビルドする事で作成したデータを確認できます。  
batファイルで[mucom88.exe]を使っています。PATHの通った場所に配置してください。  

.\mucom  
|   bintosMub.exe : .mubファイルをMUCOM88mdz(BGM)に変換  
|   bintosWav.exe : .wavファイルをMUCOM88mdz(WAV)に変換  
|   cutoutMub.exe : .mubファイルをMUCOM88mdz(SE)に変換  
|   
+---mub  
|       mub_list.txt  : 変換する.muc(BGM)ファイルのリスト  
|       muc2md.bat    : .muc → .mub → MUCOM88mdzに変換・MD向けに変換してsrcフォルダにコピー  
|   
+---pcm  
|       pcm2md.bat  : .wav → MUCOM88mdzに変換・MD向けに変換してsrcフォルダにコピー    
|       pcm_list.txt  : 変換する.wav(PCM)ファイルのリスト  
|   
----se  
        se2md.bat   : .muc → .mub → MUCOM88mdzに変換・MD向けに変換してsrcフォルダにコピー    
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
    - Zbutton  
    mubのみ4倍速で演奏します。曲の後半確認などに使うデバッグ機能です。

  - 挙動について  
    - BGM  
    曲名は.mubのファイル名から拡張子を除いたものです。  
    非ループ曲は演奏が終わるとステータスがPLAY→STIOPに変わります。  
    ループ曲は1回目の大ループが発声すると[looped]フラグが立ちます。  
    - SE  
    SEはBGM演奏中のDCSGチャンネルに割り込みます。SE演奏が終わるとDCSGチャンネルはBGMの次の音符から再開します。  
    その為、全音符がSE割り込まれると無音期間が長くなります。  
    - PCM  
    PCMリクエストが発声すると、BGM/SEの演奏を止めてPCMを演奏します。  
    PCM演奏時にBGMリクエストが発声するとPCMを停止してBGM演奏を開始しますがSEリクエストは無視しています。  

  - Noise  
  P		[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ)  
  w		[SSG] ノイズ周波数 (0～31)  
  mucom88mdz_drv.s80に、wパラメータ変換テーブルTP5BitToDcsg12BItTblが有ります。SSGからDCSGへのノイズ周波数の調整が可能です。
  

