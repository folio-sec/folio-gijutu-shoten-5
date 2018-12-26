= 言語サービスの世界へようこそ

== 言語サービスとは

いきなりですが、「言語サービス」と聞いて何を想像したでしょうか。
Google翻訳のようなサービスでしょうか。それとも、word2vecのような自然言語処理エンジン？

残念ながらどちらも違います。本稿の文脈における「言語」とは「プログラミング言語」のことです。
そして、ここでの言語サービスとは、「プログラマにとって有益な情報を提供するプログラミング言語のサポート機能」を指します。

もう少し具体的に例示しましょう。
たとえば、あなたが何らかのプログラム（C言語でもRustでもなんでも構いません。好きな言語を想像してください）を記述していたとして、次のサポートを期待するのは何ら不自然ではないでしょう。

 * 文法の誤りを指摘してくれる
 * 宣言されていない変数を参照しようとすればエラーを表示してくれる
 * 変数名の先頭をタイプすると、補完候補を表示してくれる
 * etc...

逆に上記のようなサポートが一切存在しない状態で、正常に動作するプログラムを記述できますか？
とてもじゃありませんが、筆者にはその自信がありません。おそらくコンパイルを通すことすらできないと思います。

== 言語サービスとエディタ

先述したように、プログラマにとって言語サービスはとても馴染み深い機能です。

ところで、先に例示したエラー表示や補完といった機能から、「言語サービスはIDEが提供している」と思ったかもしれません。
確かに、一般的にIDEと呼ばれる製品にはこれらの機能は当然のように実装されています。
また、VimやAtomなどのプラグイン機構が備わっているテキストエディタでは、プログラマを支援するためのさまざまなプラグインが提供されています。

それでは、言語サービスはエディタやIDEの機能ということになるのでしょうか？
その答えはNoです。

本稿においては、言語サービスとIDEの機能は別物であるという立場で話を進めていきます。
言語サービスは、特定のエディタやIDEには依存せずに、特定のプログラミング言語向けの機能を提供すべきです。
言語サービス側とエディタ側で責務を分担することで、それぞれのエディタが機能を追加するコストを大幅に削減できます。
ひいてはプログラマは自分の書きたい言語と、利用したいエディタの組み合わせを自由に選択できるのです。

 * 言語サービス：プログラミング言語の字句解析や抽象構文木の構築、構文エラーのチェックなどを行う
 * エディタ・IDE：言語サービスが提供する機能をユーザーインターフェイスとして実装する


== TypeScriptと言語サービス

言語サービスが利用されている例として、TypeScript@<fn>{typescript}をみていきます。
TypeScriptはMicrosoftが開発・メンテナンスしているプログラミング言語です。
ECMAScript（JavaScript）のスーパーセットであり、構造的部分型による静的な型付けが行えるのが特徴です。

本稿の後半では、Language Server Protocol（LSP）という汎用的な言語サービスの仕組みについて紹介しますが、LSPはTypeScriptで採用されたtsserverという技術をベースに策定されています。

余談ですが筆者がTypeScriptを愛してやまないのも、このtsserverという仕組みがあるからに他なりません。

さて、TypeScriptのアーキテクチャにおけるtsserverの位置づけは@<img>{ts_architecture}のようになっています。

//image[ts_architecture][TypeScriptにおけるtsserver @<href>{https://github.com/Microsoft/TypeScript/wiki/Architectural-Overview#layer-overview} より][scale=1.0]{
//}

図中でLanguage Serviceと書かれているのが、TypeScriptの言語サービス実装に相当します。
Language Serviceの型定義は@<href>{https://github.com/Microsoft/TypeScript/blob/v3.0.0/src/services/types.ts#L251}に記載があります。@<list>{ts_lang_service}にその一部を記載します。
言語サービスの役割として述べたとおり、エディタ向けのAPIが列挙されているのがわかります。

//listnum[ts_lang_service][Language Serviceのインターフェイス（一部抜粋）]{
interface LanguageService {

  // 補完可能なキーワード候補の取得
  getCompletionsAtPosition(fileName: string, position: number): CompletionInfo;

  // QuickFixの取得
  getQuickInfoAtPosition(fileName: string, position: number): QuickInfo;

  // インデント幅の取得
  getIndentationAtPosition(fileName: string, position: number,
      options: EditorOptions | EditorSettings): number;
  // etc...
}
//}

@<img>{ts_architecture}において、Language ServiceとEditorの中間にtsserverが登場しています。
tsserverはエディタがLanguage Serviceへ簡単にアクセスできるように設けられたアダプタです。
TypeScriptやその構成物であるLanguage ServiceはNode.jsなどのJavaScript実行エンジンから利用可能なAPIを提供しますが、エディタのプラグイン開発でかならずJavaScriptエンジンが利用可能である保証はありません@<fn>{ts_without_node}。
このため、どのようなエディタからでも利用しやすいよう、tsserverが標準入出力を介してLanguage Serviceを操作可能にしています。

実際にtsserverを利用しているエディタとして、MicrosoftのVisual Studio Code@<fn>{vsc}や、Vim pluginであるtsuquyomi@<fn>{tsuquyomi}@<fn>{tsuquyomi_quramy}、emacsプラグインのtide@<fn>{tide}などがあります。

//footnote[typescript][@<href>{https://www.typescriptlang.org}]
//footnote[ts_without_node][実際のところ、TypeScriptのLanguage ServiceはNode.jsではない環境（Webブラウザなど）で動作させることも可能です]
//footnote[vsc][@<href>{https://code.visualstudio.com/}]
//footnote[tsuquyomi][@<href>{https://github.com/Quramy/tsuquyomi}]
//footnote[tsuquyomi_quramy][tsuquyomiは筆者が開発・メンテナンスしているプラグインです]
//footnote[tide][@<href>{https://github.com/ananthakumaran/tide}]

=== tsserverの利用方法

ここからは、tsserverを利用して実際にTypeScriptの言語サービスを操作してみましょう。

なお、tsserverはNPMで配布されているTypeScriptのパッケージに同梱されています。
@<list>{install_ts}のコマンドでインストールしてください。

//list[install_ts][TypeScriptのインストール]{
$ npm install -g typescript
//}

まずはサンプルとなるTypeScriptのソースコードが必要です。
@<list>{sample_main_ts}の内容をmain.tsという名前で保存してください。

//listnum[sample_main_ts][main.ts]{
function hoge(x: string) {
  console.log("x", x);
}
//}

続いて、tsserverを起動します（@<list>{boot_tsserver}）。

//list[boot_tsserver][tsserverの起動]{
$ tsserver
//}

コンソールに@<list>{tss_output_first}のように出力されているはずです（pidの値は実行環境によって異なります）。
この出力の中身はあまり気にしないでください。

//list[tss_output_first][tsserverの出力結果]{
Content-Length: 76

{"seq":0,"type":"event","event":"typingsInstallerPid","body":{"pid":73404}}
//}

コンソールではtsserverコマンドは終了されておらず、入力を待ち受けている状態になっているはずです。
適当にエンターキーなどを押下すると、@<list>{ts_output_format_error}のようにエラーが出力されます。

//list[ts_output_format_error][エラー出力]{
Content-Length: 971

{"seq":0,"type":"response","command":"unknown","success":false,"message":"Error..."
//}

これは、正しいフォーマットでない値をtsserverへ入力してしまっているからです。

tsserverへの入力は以下を満たしている必要があります。

 * JSON文字列であること
 * commandという命令を表すキーを有していること
 * commandの引数は、argumentsというキーに格納すること

サンプルとして、@<list>{sample_open_cmd}を入力してみてください。

//list[sample_open_cmd][tsserverの入力値例]{
{"command":"open","arguments":{"file":"main.ts"}}
{"command":"quickinfo","arguments":{"file":"main.ts","line":2,"offset":20}}
//}


@<list>{sample_open_cmd}1行目のopenコマンドでは何も出力されず、2行目のquickinfoコマンド実行後に@<list>{sample_out_raw_quickinfo}のように出力されたはずです。

//list[sample_out_raw_quickinfo][quickinfo出力]{
Content-Length: 241

{"seq":0,"type":"response","command":"quickinfo","success":true,"body": "...略" }
//}

ここまで見てきたように tsserverの出力は以下から構成されています。

 * @<code>{Content-Length: <整数>}からなるヘッダーパート
 * JSON文字列であるコンテンツパート

さきほどの@<list>{sample_open_cmd}の出力について、コンテンツパートを整形したものが@<list>{quickinfo_out_jq}です。

//listnum[quickinfo_out_jq][quickinfo出力（整形後）]{
{
  "seq": 0,
  "type": "response",
  "command": "quickinfo",
  "success": true,
  "body": {
    "kind": "parameter",
    "kindModifiers": "",
    "start": {
      "line": 2,
      "offset": 20
    },
    "end": {
      "line": 2,
      "offset": 21
    },
    "displayString": "(parameter) x: string",
    "documentation": "",
    "tags": []
  }
}
//}


今回、tsserverへ送信したコマンド（@<list>{sample_open_cmd}）は次の意味を持っています。

 1. main.tsをopenコマンドによりtsserverで開かせる
 2. quickinfoコマンドにより、main.tsの2行目の20文字目のカーソル上の情報を取得

ここで、サンプルのソースコードとして用意したmain.ts（@<list>{sample_main_ts}）を思い出してください。
「main.tsの2行目の20文字目」は@<code>{ console.log("x", x) }におけるxという変数を参照している箇所に一致します。
そして、そのコマンド実行結果（@<list>{quickinfo_out_jq}）では@<code>{"displayString": "(parameter) x: string"}として「xはパラメータであり、その型は文字列型である」ことを正しく教えてくれました。

=== 利用可能なコマンド

先述のquickinfoコマンドは、カーソル上の識別子について簡単な情報を取得するためのコマンドでした。
このコマンドはエディタ側からはツールチップのようなユーザーインターフェイスを実装するために用いられます。

quickinfoコマンド以外にも、tsserverにはカーソル上の情報を取得するコマンドが数多く用意されています。その一部を@<table>{tsserver_cursor_commands}に記します。

//tsize[|latex|25,75]
//table[tsserver_cursor_commands][tsserverの情報取得系コマンド]{
コマンド名	内容
-----------------------------------------------
completions	カーソル位置における補完可能な単語の一覧を取得
definition	カーソル位置の識別子の定義箇所を取得
implementation	カーソル位置のインターフェイスや抽象クラスについて、対応する実装の一覧を取得
occurrences	カーソル位置の識別子の出現箇所一覧を取得
quickinfo	カーソル位置の識別子の簡単な情報を取得
references	カーソル位置の識別子を参照している箇所の一覧を取得
rename	カーソル位置の識別子を別名に変更する場合に、同時に変更しなくてはならない箇所の一覧を取得
signatureHelp	カーソル位置の関数・メソッドについて、引数などの呼び出しに必要な情報を取得
typeDefinition	カーソル位置の識別子について、その型の定義箇所を取得
//}

tsserverで利用可能なコマンドの完全な一覧が必要な場合、@<href>{https://github.com/Microsoft/TypeScript/blob/v3.0.0/src/server/protocol.ts}を参照してください。

=== ワークスペースの状態管理

ところで、先述の@<list>{sample_open_cmd}のコマンド入力例ではquickinfoコマンドを実行する前に、openというコマンドを実行していました。

//emlist{
{"command":"open","arguments":{"file":"main.ts"}}
{"command":"quickinfo","arguments":{"file":"main.ts","line":2,"offset":20}}
//}

もし、いきなりquickinfoコマンドを実行してしまうと、tsserverはエラーを出力してしまいます。
これはquickinfoコマンドだけでなく、@<table>{tsserver_cursor_commands}に記載した他のコマンドでも同様です。

なぜopenコマンドが必要なのでしょうか。

tsserverの背後で動作しているTypeScriptのLanguage Serviceは、ファイルの状態についてステートフルなプログラムです。
Language Serviceはユーザー（エディタ）が編集しているファイルをメモリ上に展開し、ファイルのAST（抽象構文木）を構築します@<fn>{documentRegistory}。
quickinfoなどのコマンドは、すでにメモリ上に展開したASTを利用して解析を行うことで、高速にエディタへ情報を返却できるのです。
このため「Language Serviceへファイルを開かせる」という命令が、openコマンドとして明示的に設けられているのです。

Language Serviceがファイル情報をメモリに展開するということは、ユーザーが利用中のエディタにおける個々のファイルの編集状態が同期されている必要があることを意味します。
エディタで編集しているファイルの内容と、Language Serviceが認識している（メモリに展開している）ファイルの内容に齟齬が生じると、quickinfoコマンドなどの引数で渡されるカーソル情報が指し示す位置が意味をなさなくなってしまうからです。

ファイルの状態をtsserverへ通知するコマンドとして、@<table>{tsserver_text_sync_commands}が用意されています。

//tsize[|latex|25,75]
//table[tsserver_text_sync_commands][tsserverのファイル操作系コマンド]{
コマンド名	内容
----------------------------------------------------------------
change	open済のファイルについて、編集箇所を通知する
close	open済のファイルを閉じる
open	新しくファイルを開く
relaod	open済のファイルの内容を新しい内容で全置換する
saveto	tsserverが認識しているファイルの内容をファイルシステム上へダンプする。デバッグ用のコマンド
//}

@<table>{tsserver_text_sync_commands}のコマンドを実行しても、特に戻り値を返しません。
これらのコマンドはすべて、エディタ側では既知の情報を一方的にLanguage Serviceへ教えれば十分なことを考えると、戻り値が存在しないことについては得心がいくと思います。

//footnote[documentRegistory][実装上はDocumentRegistoryという機能に委譲されています]

=== エラー情報とevent

tsserverにおいて、プログラムのエラー情報の扱いは少し特殊です。

quickinfoコマンドやdefinitionコマンドは、コマンド実行後、即座に応答が返ってくるコマンド、すなわち同期的なコマンドでした。

一方で、プログラムのエラー情報の取得には非同期なコマンドを用います。
例として、@<list>{sample_error_ts}を考えてみます。これは関数hogeを定義しているものの、関数宣言の末尾閉じ括弧が抜けているため、TypeScriptの文法上エラーとなります。

//listnum[sample_error_ts][main.ts（エラー含む）]{
function hoge(x: string) {
  console.log("x", x);
//}

特定のファイルについて、エラー情報を取得する場合、tsserverへgeterrorコマンドを送信します。
しかし、geterrorコマンド自体は戻り値を持ちません。
代わりに、geterrorコマンド実行後しばらくすると（といっても数十ミリ秒ですが）、@<list>{error_event_res}のような出力を得ます。

//list[error_event_res][エラーイベント出力]{
Content-Length: 203

{"seq":0,"type":"event","event":"syntaxDiag","body":{...略}}
//}

この出力を整形すると@<list>{error_event_formatted}のようになります。

//listnum[error_event_formatted][エラーイベント（整形後）]{
{
  "seq": 0,
  "type": "event",
  "event": "syntaxDiag",
  "body": {
    "file": "main.ts",
    "diagnostics": [
      {
        "start": {
          "line": 2,
          "offset": 19
        },
        "end": {
          "line": 2,
          "offset": 19
        },
        "text": "'}' expected.",
        "code": 1005,
        "category": "error"
      }
    ]
  }
}
//}

@<list>{error_event_formatted}の17行目にあるとおり、閉じ括弧が抜けていることを指摘してくれました。

もう一点、ここで注目すべきは、@<code>{ "type": "event" }という部分です。
eventは、tsserverからクライアントへ一方的に何かしらのイベントを通知することを示すタイプです。

geterrorコマンドの場合、@<table>{tsserver_diag_events}に示すイベント発行を予約します。

//tsize[|latex|25,75]
//table[tsserver_diag_events][geterrが発行するイベント]{
イベント名	内容
----------------------------------------------------------------
semanticDiag	型の誤りなど、TypeScript上の意味的な誤りの指摘
suggestionDiag	非推奨なコードの書き方の指摘
syntaxDiag	構文エラーの指摘
//}

なお、エラー情報以外にもいくつかのイベントが定義されていますが、本稿ではその詳細は割愛します。

=== クライアントコマンドフロー

ここまで見てきた内容の総括として、tsserverのクライアントとなるエディタプラグインが実行するコマンドのフローを次にまとめます。

 * エディタの起動に合わせて、tsserverのプロセスを起動する
 * エディタでファイルを開いたら、openコマンドを実行してtsserverにファイルを開かせる
 ** エディタ上のファイルが編集されたら、changeコマンド（またはreloadコマンド）でtsserverへ変更内容を通知する
 ** ユーザーアクションに応じて、quickinfoコマンドやcompletionsコマンドを実行し、エディタ上のユーザーインターフェイスへ反映する
 ** geterrコマンドでトリガーされたイベントが通知されたら、エラー情報をエディタ上のユーザーインターフェイスへ反映する
 * ユーザーがファイルの編集を終えたら、closeコマンドを実行し、tsserver上のファイルを閉じる
 * エディタの終了に合わせてtsserverのプロセスを終了させる

=== tsserverの拡張

本節ではTypeScriptの言語サービスであるLanguage Serviceとそのアダプタとなるtsserverについて、利用側、すなわちエディタプラグインから見た解説をしてきました。
一方で、TypeScriptのLanguage Serviceはその機能を拡張することも可能です。

TypeScriptのバージョン2.3にて、Language Service Extensibilityと呼ばれる機能が追加されました@<fn>{lse}。
この機能を用いると、ユーザーはLanguage Serviceの任意のメソッドをオーバーライドできます。

Language Serviceの拡張には、プラグインと呼ばれる形式でプログラムを用意します。
簡単なプラグインの例を@<list>{first_extension}に用意しました。


//listnum[first_extension][Language Service Pluginの実装例]{
/* my-first-plugin.ts */
import * as ts from 'typescript/lib/tsserverlibrary';

function factory (mod: { typescript: typeof ts } ) => {
  const pluginModule: ts.server.PluginModule = {
    create,
  };
  return pluginModule;
};

function create (info: ts.server.PluginCreateInfo): ts.LanguageService {
  const ls = info.languageService;

  // オリジナルのメソッドを退避しておく
  const delegate = ls.getQuickInfoAtPosition;

  // tooltip用のメソッドを上書き
  ls.getQuickInfoAtPosition = (fileName: string, position: number) => {
    const result = delegate(fileName, position); // 元メソッドを呼び出す
    if (!result.displayParts || !result.displayParts.length) return result;
    // 結果を修正する
    result.displayParts = [
      { kind: "", text: "!!" },
      ...result.displayParts,
      { kind: "", text: "!!" },
    ];
    return result;
  };

  return ls;
}

export = factory;
//}

@<list>{first_extension}では、LanguageServiceインターフェイスのgetQuickInfoAtPositionメソッドについて、表示されるテキストの前後を「!!」で囲む処理を行っています。
tsserverでの実行の場合、quickinfoコマンドの戻り値に相当します。

Language Serviceのプラグインを利用する際は、TypeScriptのプロジェクト定義を行うtsconfig.jsonにプラグインを実装したファイルのパスを指定します（@<list>{tsconfig_with_plugin}）。

//listnum[tsconfig_with_plugin][Language Serviceプラグインの利用設定]{
{
  "compilerOptions": {
    /* 中略 */
    "plugins": [
      { "name": "./my-first-plugin" }
    ]
  }
}
//}

実際に、いくつかのプラグインが提供されています。
その一部を@<table>{plugins}に記載します。

//tsize[|latex|45,65]
//table[plugins][利用可能なプラグイン]{
NPMパッケージ名	機能
----------------------------------------------------------------
@angular/language-service	Angularのテンプレートについて、補完やエラーチェックなどの機能を追加する
vue-ts-plugin	Vue.jsのシングルファイルコンポーネントの<script>セクションについて、補完やエラーチェックを提供する
tslint-language-service	エラーチェックにtslintの内容を追加する
ts-graphql-plugin	GraphQL形式のテンプレートストリングについて、エラーチェックと補完機能を追加する
//}

残念ながら、それほど多くのプラグインが存在しているわけではないため、我こそは！という方がいたら、是非新しいプラグインを実装・公開してみてください。

//footnote[lse][@<href>{https://github.com/Microsoft/TypeScript/wiki/Roadmap#23-april-2017}]

== Language Server Protocol

=== LSPとは

エディタとは独立した機能として、言語サービスが提供されることによって、各エディタのプラグイン開発者は言語サービスを利用する部分のみを実装すればよくなります。

エディタをフロントエンド、言語サービスをバックエンドと捉えると、まるで最近のアプリケーション開発のトレンドとそっくりですね。
それぞれのレイヤーについて、そのレイヤーのスペシャリストが担当して開発を行うことで開発効率も品質も高まるのです。

しかし、課題はまだ残っています。

複数の言語サービスを利用するケースを考えてみましょう。
各言語サービスが無秩序にサービスの機能をAPIとして公開すると、エディタプラグインは各言語サービスごとにAPI呼び出しを実装しなくてはなりません。
言語サービスの機能は「補完情報の提供」や「エラー情報の提供」などが挙げられますが、これらの情報をどのようにエディタ上のUIへ反映するかについては、プログラミング言語の種類を問わずに、そのエディタの中で統一的なUIであることを考慮すると、言語サービスのAPI呼び出し部分がバラバラなのは苦痛でしかありません。

これは裏を返すと、複数の言語サービス間でAPIが統一されていればよいということです。

この問題を解決すべく生まれたのがLanguage Server Protocol（LSP）@<fn>{lsp}です。

LSPは言語サービスを提供するサーバーと、そのクライアント（エディタプラグイン）間のAPIを定めた仕様です。
Microsoftが2016年にオープンソースプロジェクトとして公開しています@<fn>{lsp_release_blog}。
C#の言語サービスであるOminiSharp@<fn>{omnisharp}やTypeScriptのtsserver、そしてVisual Studioの開発で蓄積してきたノウハウの1つの完成形と言ってもよいでしょう@<fn>{lsp_background}。

LSPという統一されたAPI仕様を介することによって、次の分解が達成されます（@<img>{lsp_and_editors}）。

 * 言語サービス：LSPにしたがって言語情報を提供する
 * エディタプラグイン：LSPクライアントとして、APIから受け取った情報をUIに反映する

//image[lsp_and_editors][LSPのもたらす効果 @<href>{https://code.visualstudio.com/docs/extensions/example-language-server} より][scale=0.9]{
//}

これにより、一度エディタ側にLSPクライアントが実装されれば、利用者であるプログラマはどの言語サービスを利用するかを追加するだけで、新しい言語サービスの恩恵をすぐに享受できます。

//footnote[lsp][@<href>{https://microsoft.github.io/language-server-protocol}]
//footnote[omnisharp][@<href>{http://www.omnisharp.net/}]
//footnote[lsp_release_blog][@<href>{https://code.visualstudio.com/blogs/2016/06/27/common-language-protocol}]
//footnote[lsp_background][今は読むことはできないのですが、LSP公開当時、GitHubのレポジトリには「LSPは虚空から誕生したわけではない。我々の長年における努力と失敗の結晶なのだ」という趣旨の文章が掲載されており、筆者は胸が激アツになったことを今でも覚えています]

=== 各言語のLSP実装状況

LSPを実装している言語サービスはLSP公式サイトの「Implementations」というセクション@<fn>{lsp_impl}に一覧が記載されています（@<img>{lsp_impl_servers}）。
本稿の執筆時点で70を超える言語サービスのLSP実装が公開されています。

たとえば、C++の言語サービスであるclangd@<fn>{clangd}やGraphQLの言語サービスであるgraphql-language-service@<fn>{graphql_Language_service}のように、公式にLSPを実装したサーバーが提供されているケースもあります。

//image[lsp_impl_servers][LSPのサーバー実装 https://microsoft.github.io/language-server-protocol/implementors/servers より][scale=0.7]{
//}

余談ではありますが、本稿の前半で解説したTypeScriptの言語サービスについては、現状ではMicrosoft公式のLSP実装は存在しません。
typescript-language-server@<fn>{typescript_language_server}や、javascript-typescript-langserver@<fn>{javascript_typescript_langserver}など、いくつかのLSP実装は公開されているのですが、これらはいずれも3rdパーティ製です。
2016年9月の時点で、TypeScriptのGitHubレポジトリにはこの件についてのissue@<fn>{ts_lsp_issue}がopenされているのですが、実装が進むことなく今日まで至っています。

//footnote[lsp_impl][@<href>{https://microsoft.github.io/language-server-protocol/implementors/servers}]
//footnote[clangd][@<href>{https://clang.llvm.org/extra/clangd.html}]
//footnote[graphql_Language_service][@<href>{https://github.com/graphql/graphql-language-service}]
//footnote[typescript_language_server][@<href>{https://github.com/theia-ide/typescript-language-server}]
//footnote[javascript_typescript_langserver][@<href>{https://github.com/sourcegraph/javascript-typescript-langserver}]
//footnote[ts_lsp_issue][@<href>{https://github.com/Microsoft/TypeScript/issues/11274}]

=== 各エディタのLSPサポート状況

クライアントであるエディタ側についても、公式サイトに記載があります（@<img>{lsp_impl_clients}）。
多くのエディタがLSPをサポートしているので、ご自身で利用しているエディタにもLSPプラグインを是非導入してみてはいかがでしょうか。

//image[lsp_impl_clients][LSP対応ツール @<href>{https://microsoft.github.io/language-server-protocol/implementors/tools}より][scale=0.7]{
//}

=== Base Protocol
ここからはLSPの詳細をもう少し見ていきましょう。
なお、本稿では執筆時点での最新バージョンであるLSP 3.13.0にしたがって解説していきます。

言語サーバーとクライアント間の通信はすべてLSP Base Protocolに従う必要があります。

LSP Base Protocolの電文は、ヘッダーパートとコンテンツパートから構成され、これらは改行コード（CRLF）で区切られていなければなりません。
ヘッダーパートは @<code>{Content-Length: <数値><CRLF>} の形式であり、ここにコンテンツパートのバイト長を格納しています。
コンテンツパートはJSON文字列である必要があります。

具体的には、@<list>{base_protocol_example}のような形式になります。

//list[base_protocol_example][LSP Base Protocolの例]{
Content-Length: 241

{"jsonrpc": "2.0", "method": ...略}
//}

なにやら見覚えのある形式ではありませんか？
本稿の前半で解説してきたtsserverの応答電文（@<list>{tss_output_first}）と同じ形式ですね。

tsserverの場合、サーバーからクライアントへの通信のみにヘッダーパートが存在し、クライアントからの通信はコンテンツであるJSONのみでした。
一方、LSPの場合は、クライアント <-> サーバーのどちらの方向のやりとりでも、上記のBase Protocolの形式でなくてはなりません。

LSP Base Protocolは、テキストベースである点やCRLF区切りを利用するという意味において、HTTPと類似性のあるプロトコルです。
HTTPはトランスポートにTCPを用いますが、LSPはトランスポートに何を使うかについては一切の規程がありません。
主要なサーバーをみると、tsserverと同じく標準入出力を利用した実装が多いようです@<fn>{lsp_socket}。

//footnote[lsp_socket][筆者は実際に利用することはないですが、ソケットをサポートしている実装も見受けられます]

=== JSON RPC

コンテンツパートであるJSON文字列について、さらに詳しく見ていきましょう。

LSPのコンテンツパートは@<list>{jsonrpc_example}に示すような、JSON RPC@<fn>{jsonrpc}というRemote Procedure Callの仕様に沿った形式である必要があります。

//listnum[jsonrpc_example][JSON RPCの例]{
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "rootUri": "file:///Users/quramy/sample_ws",
    "initializationOptions": {},
    "capabilities": {},
    "trace": "off"
  }
}
//}

それぞれのプロパティの意味は@<table>{jsonrpc_req_keyvalue}のとおりです。

//tsize[|latex|25,75]
//table[jsonrpc_req_keyvalue][JSON RPCのリクエストパラメータ]{
プロパティ名	意味
----------------------------------------------------------------
jsonrpc	JSON RPCのバージョン。LSPでは2.0固定
id	呼び出し元が付与するリクエスト毎に一意な整数。省略した場合、このリクエストが応答不要であること意味する
method	呼び出したいメソッドの名前
params	メソッドの引数となるオブジェクト。省略可能
//}

本稿の前半で、tsserverには3種類の通信パターンが存在することを解説しました。
このパターンはTypeScriptという言語には依存しておらず、言語サービスとクライアント間の通信を汎用的に考えても必要となるパターンです。

 * クライアントからサーバーへの戻り値のあるリクエスト（例：definitionコマンド）
 * クライアントからサーバーへの一方的な通知（例：openコマンドやchangeコマンド）
 * サーバーからクライアントへの一方的な通知（例：diagnosticsイベント）

JSON RPCの仕様であれば、これら3種の通信パターンを満たせることがわかります。
idの有無によって、その呼出が応答を求めているリクエストなのかイベントの通知なのかを判別できるからです。

JSON RPCリクエストが応答を要求した場合、対応するレスポンスは@<table>{jsonrpc_res_keyvalue}のプロパティを持ったオブジェクトになります。

//tsize[|latex|25,75]
//table[jsonrpc_res_keyvalue][JSON RPCのレスポンスパラメータ]{
プロパティ名	意味
----------------------------------------------------------------
jsonrpc	JSON RPCのバージョン。LSPでは2.0固定
id	リクエストのidに一致した整数
result	戻り値の本体となるオブジェクト
error	メソッド呼び出しが失敗した場合に付与されるエラーオブジェクト。code, message, dataから構成される
//}

//footnote[jsonrpc][@<href>{https://www.jsonrpc.org/specification}]

=== 具体例

ここまででLSPの基本的なプロトコルについては解説してきたので、ここからは実際のLSPメソッドの具体例を見ていきましょう。

本節では、RustのLSP実装であるrls@<fn>{rls}を題材とします。
rlsは@<list>{install_rls}のコマンドでインストールできます。

//list[install_rls][rlsのインストール]{
$ rustup component add rls-preview rust-analysis rust-src
//}

正常にインストールされると、@<list>{boot_rls}でrlsを起動できるようになっているはずです。

//list[boot_rls][rlsの起動]{
$ rls
//}

rlsはtsserverと同じく、標準入出力をインターフェイスとした言語サービスです。

次に、サンプルとして@<list>{sample_rust_main}に示すソースコードを用意します。
2つの整数の和を求める関数を呼び出すだけのとてもシンプルなソースコードです。

//listnum[sample_rust_main][src/main.rs]{
fn sum(x: u32, y: u32) -> u32 {
    x + y
}

fn main() {
    let result = sum(10, 10);
    println!("result: {}", result);
}
//}

それでは、rlsにLSPリクエストを送信していきます。


LSPにおける決まりごととして、LSPサーバーに最初に送信するリクエストはinitializeメソッドでなくてはいけません。
initializeメソッドのリクエストは@<list>{rls_sample_init_req}のようになります。

なお、以降ではLSPの電文について、コンテンツパートを整形した内容を記載していきます。
実際にrlsの標準入力へ書き込む場合は、コンテンツパートのJSONを1行にした上で、@<list>{base_protocol_example}のようにContent-Lengthを付与して送信してください。

//listnum[rls_sample_init_req][initializeメソッドのリクエスト]{
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "rootUri": "file:///Users/quramy/lsp-study/examples/rls",
    "capabilities": {},
    "trace": "off"
  }
}
//}

initializeメソッドには次の役割があります。

 * クライアントから言語サービスへワークスペースの情報を送信する
 * 言語サービスからクラインアントへ、サービスが対応している機能を通達する

rootUriの値である @<code>{/Users/quramy/lsp-study/examples/rls} はワークスペースのルートディレクトリです。
この値は筆者が @<code>{cargo new} で作成したRustプロジェクトのディレクトリなので、必要に応じでご自身のディレクトリに読み替えてください。

@<list>{rls_sample_init_req}では、 @<code>{id: 1} としてidパラメータが付与されています。
JSON RPCの「idパラメータの有無が応答の有無を決定する」というルールを思い出してください。
このルールのとおり、initializeメソッドには対応する応答が存在します。

rlsからはinitializeメソッドの応答として@<list>{rls_sample_init_res}が返却されます。

//listnum[rls_sample_init_res][initializeメソッドの応答]{
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "capabilities": {
      "textDocumentSync": 2,
      "hoverProvider": true,
      "completionProvider": {
        "resolveProvider": true,
        "triggerCharacters": [
          ".",
          ":"
        ]
      },
      "definitionProvider": true,
      "referencesProvider": true,
      "documentHighlightProvider": true,
      "documentSymbolProvider": true,
      "workspaceSymbolProvider": true,
      "codeActionProvider": true,
      "documentFormattingProvider": true,
      "documentRangeFormattingProvider": false,
      "renameProvider": true,
      "executeCommandProvider": {
        "commands": [
          "rls.applySuggestion",
          "rls.deglobImports"
        ]
      }
    }
  }
}
//}

応答の中身はcapabilitiesというオブジェクトが1つあるだけですね。
このcapabilitiesが、先述した「サービスが対応している機能」に相当します。

たとえば、@<list>{rls_sample_init_res}には @<code>{"definitionProvider": true} とあります。
definitionProviderは、コード上の識別子について、その定義がどこに存在するかを提供する機能です。
本稿前半で解説したtsserverでは、definitionコマンドに相当します。

一方で、 @<code>{"documentRangeFormattingProvider": false} とあるため、rlsはdocumentRangeFormattingという機能には対応していないことがわかります。
ちなみに、documentRangeFormattingは、ソースコードの部分的な範囲を指定して、その範囲のみを整形するための命令です。
textDocument/rangeFormattingというメソッド名が割当らています。

このようにcapabilities@<fn>{server_capabilities}の内容から言語サービスが何ができて何ができないのかを受け取ることで、クライアント側ではどのメソッドを使うのかを決定するのです。

クライアントとrls間の初期化が完了したので、続いてrlsにtextDocument/didOpenというメソッドを送信します（@<list>{rls_sample_didopen_req}）。

//listnum[rls_sample_didopen_req][didOpenメソッドのリクエスト]{
{
  "jsonrpc": "2.0",
  "method": "textDocument/didOpen",
  "params": {
    "textDocument": {
      "uri": "file:///Users/quramy/lsp-study/examples/rls/src/main.rs",
      "version": 1,
      "languageId": "rust",
      "text": "fn sum(x: u32, y: u32) -> ...(略)"
    }
  }
}
//}

このメソッドは言語サービスに対して、クライアントであるエディタがファイルをオープンしたことを通知するためのメソッドです。
@<list>{rls_sample_didopen_req}では紙面の都合上、省略していますが、 @<code>{textDocument.text} というパラメータにはsrc/main.rsのソースコード全文の中身を付与しなくてはなりません。

TypeScriptの言語サービスにおいても、openコマンドが存在していたことを思い出してください。
「言語サービスはファイルの状態についてステートフルであるべきだ」という思想がLSPにも受け継がれているのです。

実際の操作としては、ファイルを開くという行為はエディタ上で行われており、textDocument/didOpenというメソッド名が示すとおり、すでにこの行為はクライアント側で完了しています。
このメソッドを受け取った言語サーバーの側には成功・失敗といった概念は存在しません。
すでにクライントによって完結している動作なのですから。
したがって、このメソッドには応答は存在せず、@<list>{rls_sample_didopen_req}にもidパラメータは登場しないことに注意してください。

ここまでで、rlsに@<list>{sample_rust_main}のmain.rsの解析をさせるための準備が整いました。
最後にtextDocument/definitionメソッドを使ってみましょう（@<list>{rls_sample_definition_req}）。

//listnum[rls_sample_definition_req][textDocument/definitionメソッドのリクエスト]{
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "textDocument/definition",
  "params": {
    "textDocument": {
      "uri": "file:///Users/quramy/lsp-study/examples/rls/src/main.rs"
    },
    "position": {
      "line": 5,
      "character": 17
    }
  }
}
//}

@<list>{rls_sample_definition_req}は、main.rsの6行目18列目のカーソル位置について、その定義元はどこにあるのかを問い合わせています。
LSPでは、ファイル内の位置を表す際、0始まりでカウントするため、6行目であれば @<code>{"line": 5} のように指定します。

main.rsの6行目とは、

//emlist{
    let result = sum(10, 10);
//}

でしたので、ここではこのsumという関数の定義元を問い合わせていることになります。
正しくrlsが定義元を返してくれているかどうか、応答を確認してみましょう（@<list>{rls_sample_definition_res}）。

//listnum[rls_sample_definition_res][textDocument/definitionメソッドの応答]{
{
  "id": 2,
  "jsonrpc": "2.0",
  "result": [
    {
      "uri": "file:///Users/quramy/lsp-study/examples/rls/src/main.rs",
      "range": {
        "start": {
          "line": 0,
          "character": 3
        },
        "end": {
          "line": 0,
          "character": 6
        }
      }
    }
  ]
}
//}

main.rsの1行目の4~7文字目にsum関数の定義が存在していることを正しく教えてくれました。


//footnote[rls][@<href>{https://github.com/rust-lang-nursery/rls}]
//footnote[server_capabilities][正確にはServer Capabilitiyと呼ばれます。Client Capabilitiyという概念もあるのですが、本稿では説明を割愛します]

=== その他のLSPのメソッド

ここまでで解説したものも含め、LSPの全メソッドは公式サイトのSpecification@<fn>{lsp_specifications}に記載があります。
LSP公式サイトではメソッドをいくつかのカテゴリに分類しています。
各カテゴリの内容は@<table>{lsp_method_categories}のとおりです。

//tsize[|latex|25,75]
//table[lsp_method_categories][LSPメソッドのカテゴリ]{
カテゴリ名	内容
----------------------------------------------------------------
General	initializeメソッドや、送信済みリクエストをcancelするcancelメソッドなど
Window	ロギングや特定のメッセージを言語サービスからクライアントへ通知するためのメソッドについて
Telementy	性能測定用のメソッドについて
Client	Capabilitiyを動的に登録・解除するためのメソッドについて
Workspace	ワークスペース全体の設定に利用するメソッドについて
Text Syncronization	クライアントと言語サービス間でファイルを同期させるためのメソッドについて。textDocument/didOpenやtextDocument/didChangeはこのカテゴリに属している
Diagnostics	ファイルの文法エラーや型エラーなどを通知するtextDocument/publishDiagnosticsメソッドについて
Language Fetures	定義ジャンプやツールチップ、補完などいわゆる言語サービスの機能について。textDocument/definitionもこのカテゴリに属している
//}


//footnote[lsp_specifications][@<href>{https://microsoft.github.io/language-server-protocol/specification}]

=== LSPの応用事例

本節の最後に、LSPを利用したサービスを紹介しましょう。

sourcegraph@<fn>{sourcegraph}はGitHubに公開されているソースコードについて、定義ジャンプ・参照元ジャンプ・ツールチップといった機能をChrome ExtensionやFirefox addonとして提供するサービスです（@<img>{sourcegraph}）。

//image[sourcegraph][sourcegraphのツールチップ機能 @<href>{https://about.sourcegraph.com/}より][scale=1.0]{
//}

これらの機能は、LSPの下記メソッドを利用して実現されています。

 * textDocument/definition
 * textDocument/references
 * textDocument/hover

sourcegraphのGitHubレポジトリではGo言語@<fn>{sourcegraph_go_langserver}やTypeScript@<fn>{sourcegraph_ts_server}など、さまざまな言語のLSP実装が公開されています。

一方で、sourcegraphの提供するサービスのクライアントは通常のエディタではなく、GitHubを閲覧しているChrome拡張やFirefox addonです。
言語サービスのサーバ毎にgit cloneしてworkspaceを構築する、ということをしてもスケールしないですし、初期化（initializeメソッド）の待ち時間も無視できません。
このような課題を解消するために、sourcegraphのLSPでは、Text Syncronizationに関連するメソッドを一部拡張することで、言語サーバーのスケーラビリティを担保しています。
sourcegraphのブログ（@<href>{https://about.sourcegraph.com/blog/part-2-how-sourcegraph-scales-with-the-language-server-protocol/}）に詳細が記載されていますので、興味がある方は是非読んでみてはどうでしょうか。

//footnote[sourcegraph][@<href>{https://about.sourcegraph.com/}]
//footnote[sourcegraph_go_langserver][@<href>{https://github.com/sourcegraph/go-langserver}]
//footnote[sourcegraph_ts_server][@<href>{https://github.com/sourcegraph/javascript-typescript-langserver}]

== 終わりに

本稿では、プログラマがプログラムを書くためには欠かせない、言語サービスについて解説をしてきました。
前半でTypeScriptのtsserverを、後半では言語サービスの汎用プロトコルであるLSPを紹介しましたが、両者に互換性は無いものの、設計思想やクライアントコマンドフローに数多くの類似点が存在することが見てとれます。

言語サービスは、その利用者となるプログラマはとても多いものの、言語サービスそのものやエディタのクライアントを扱ったことのあるプログラマとなると、一気に数が減ってしまうのが実情です。
本稿が言語サービスの世界に最初の一歩を踏み入れるための一助になれば、これほど嬉しいことはありません。
