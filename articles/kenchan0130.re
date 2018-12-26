= macOS 端末を管理してみよう

本章では macOS の端末管理について紹介します。
builderscon tokyo 2018 で発表した「イノベーションを止めずに、端末管理と運用を行う方法」@<fn>{presentation}における macOS の内容を加筆修正したものです。

巷では、 Windows の端末管理に関して多くの資料や知見が存在しています。
これは単純に母数が多いためであると筆者は考えます。
そのため、少しでも多く macOS に関する知見を世に出せればと思い、微力ながら筆を取りました。

//footnote[presentation][https://builderscon.io/tokyo/2018/session/e05419d8-dc60-49f0-9056-3f13ae5ac68e に概要と発表資料および動画の閲覧が可能]

== 端末管理を支える Apple のソリューション
まずは、 macOS の端末管理がどのような仕組みを利用して実現されていくのかを紹介します。

//image[macos-overview][macOS 端末管理の全体像][scale=0.9]{
//}

=== Mobile Device Management
Mobile Device Management（以降 MDM）は状況によって、しばしば異なる意味をもつことがあります。
MDMは端末を管理・監視すること、または、そのため機能やサービスなどを示すことが多いです。
本章では、特に端末を管理・監視するためのサービスに関しては Enterprise Mobility Management（以降 EMM）サービスと呼ぶことにします。

Apple 社は iOS、tvOS および macOS に対して MDM をサポートするフレームワークを組み込んでいます。

具体的に可能な内容に関しては Apple 社のドキュメント @<fn>{mdm-document} をご覧ください。

//footnote[mdm-document][https://help.apple.com/deployment/mdm]

====[column] Mobile Device Management Protocol
Apple 社はサードパーティが MDM のフレームワークを使用できるように、 Mobile Device Management Protocol と呼ばれる専用のプロトコルを定義しています。

プロトコルの内容は https://developer.apple.com/enterprise/documentation/MDM-Protocol-Reference.pdf で公開されています。

==== 構成プロファイル
構成プロファイルは、端末の構成情報を記載したプロパティリストファイル@<fn>{mobileconfig}です。
このファイルが各端末に設定されることで、構成が適用されます。

//footnote[mobileconfig][.mobileconfig 拡張子の plist（xml）ファイル]

=== Apple Business Manager / Apple School Manager
Apple Business Manager および Apple School Manager は Apple 社が提供している Web サービスです。

共通の機能として、

  * 管理者のアカウント管理
  * 端末と EMM サービスの連携
  * アプリケーションと EMM サービスの連携
  * アプリケーションのライセンス管理

が提供されています。

また、 Apple School Manager では、

  * クラスの管理
  * iTunes U への名簿インポート

が提供されている他、 Apple ID 1 アカウントに対して iCloud 200GB の無料枠の提供されています。

====[column] Apple Deployment Program
Apple Deployment Program とは、 Device Enrollment Program （以降 DEP）@<fn>{DEP}と Volume Purchase Program （以降 VPP）@<fn>{VPP}を実施するためのプログラム一式です。

DEP は端末の導入を効率よく行うために、EMM サービスに自動的に登録する機能です。
また、 VPP はアプリケーションやブックを一括購入して、複数の端末および多数のユーザに配布が可能となる機能です。

この Apple Deployment Program は 2018 年に、Apple Business Manager および Apple School Manager に完全に移行しました。

しかし 2018 年 10 月現在、 Apple Business Manager および Apple School Manager においては DEP と VPP に関しての概念が各所に残っています。
Apple 社が今後、この概念を残し続けるかは定かではないため、引き続き覚えておくとよいでしょう。

//footnote[DEP][端末の導入を効率よく行うために、EMM サービスに自動的に登録する機能]
//footnote[VPP][アプリケーションやブックを一括購入して、複数の端末および多数のユーザに配布する機能]

==== 使用条件
Apple Business Manager および Apple School Manager を使用するには以下を用意する必要があります。

  * D-U-N-S® 番号
  * 英語表記の組織名
  * SMS を受け取れる電話番号

また、 Apple School Manager に関しては教育行政組織または教育行政庁であることが必要です。@<fn>{apple school manager condition}

さらに、DEP に関しても使用条件があります。

DEP を使用するには、 Apple 社から直接端末を購入する@<fn>{buy devices from apple}か、通信キャリアからの購入またはレンタルである必要があります。

ちなみに iOS に関しては、 iOS 11 以上の端末の場合、 Apple Configurator 2@<fn>{apple configurator 2} を使用することで DEP の登録が可能となりました。

//footnote[apple school manager condition][学校法人、教育委員会および PTA などが対象となり、塾などの教育サービスを提供している法人は対象外]
//footnote[buy devices from apple][Apple 法人窓口、 Apple Store 法人窓口、 Apple Premium Reseller などの Apple 正規販売店での購入]
//footnote[apple configurator 2][macOS で使用できる iOS および tvOS 端末管理用のアプリケーション]

====[column] D-U-N-S® 番号
Data Universal Numbering System の略で、ダン＆ブラッドストリート社が管理している、企業コードの付与管理システム、並びに同システムによって各企業に付与された企業コードの名称です。

発行方法は大きく 2 つあります。

ひとつ目は、 Apple 社の Web サイトで申請リクエストを出す方法です。@<fn>{D-U-N-S Number Support Apple Developer}

ふたつ目は、東京商工リサーチ社の代行サービス@<fn>{tokyo-shoukou-daikou}を使用することです。費用は約 2 万円です。
//footnote[D-U-N-S Number Support Apple Developer][D-U-N-S Number Support Apple Developer（https://developer.apple.com/support/D-U-N-S/）に詳しく記載]
//footnote[tokyo-shoukou-daikou][https://duns-number-jp.dnb.com/search/jpn/login.asp]

====[column] Apple Business Manager / Apple School Manager における Apple ID
Apple 社のサービスを使用する場合に必要になるのが、 Apple ID です。

この Apple ID ですが、 Apple Business Manager または Apple School Manager 経由せずに登録してしまうと、iTunes アカウントと紐付いてしまします。
iTunes アカウントと紐付いた Apple ID は、 Apple Business Manager または Apple School Manager では使用することができません。
逆もしかり、 Apple Business Manager または Apple School Manager アカウントに紐付いた Apple ID では、 App Store や iTunes を使用することができません。

解決方法としては

  1. 共通アカウントを用意する
  2. 新しい Apple ID を作成してメールアドレスを移管する

があります。

共通アカウントはセキュリティ的に脆弱になりがちであるため、極力避けた方がよいと筆者は考えます。
オススメは「新しいアカウントを作成してメールアドレスを移管する」方法です。
Apple ID は https://appleid.apple.com/ 上でアカウントの削除またはメールアドレスを変更することができます。
Apple ID に紐付いているメールアドレスを、捨てアドレスと連携またはアカウント自体を削除を行うと、約 30 日後に元々登録してあったメールアドレスが使用可能となります。
メールアドレスが使用可能になったタイミングで、新しい Apple ID を元々登録したあったメールアドレスで登録し直すという方法です。

== 端末管理サービス

Windows の端末管理サービスは、 Active Directory と何かしらのエンドポイントツール、または Azure Active Directory および Microsoft Intune を使用するのが一般的です。
iOS や tvOS における EMM サービスはいくつも存在しますが、 macOS ではこれと言ったサービスをあまり聞きません。

実は macOS 端末の管理ツールはいくつかありますが、特に有名なのは

  1. プロファイルマネージャ@<fn>{profile-manager}
  2. Microsoft Intune@<fn>{microsoft-intune}
  3. Cisico Meraki System Manager@<fn>{SM}
  4. Jamf PRO@<fn>{jamf}

です。

//footnote[profile-manager][https://support.apple.com/ja-jp/profile-manager]
//footnote[microsoft-intune][https://www.microsoft.com/ja-jp/cloud-platform/microsoft-intune]
//footnote[SM][https://documentation.meraki.com/SM]
//footnote[jamf][https://www.jamf.com/ja/]

//table[EMM service comparison][各 EMM サービスの比較]{
EMM サービス	サーバー管理	参考費用
-------------------------------------------------------------
プロファイルマネージャ	On Premises	¥2,400
Microsoft Intune	Clould	¥650/user/month ~
Cisico Meraki System Manager	Clould	unknown/device/month
Jamf PRO	On Premises, Clould	$6.67/device/month + $6,000
//}

=== プロファイルマネージャ
プロファイルマネージャは、 macOS Server@<fn>{macOS Server} アプリケーションに付属している EMM サービスです。
Apple 社の公式ツールであるため、必ず最新の OS に対応されます。
Apple 認定トレーニングにおいてもプロファイルマネージャを用いて講義が行われます。

プロファイルマネージャでは、

  * インベントリ管理
  * App Store の App の配布
  * 構成プロファイルの配布
  * インストール済み App 一覧
  * MDM コマンド@<fn>{MDM Command}の送信

が可能となります。
また、 Script や PKG の配布は Apple Remote Desktop@<fn>{Apple Remote Desktop} アプリケーションを使用することで実現が可能となります。

しかし、macOS Server であるため、ワークステーションを準備してかつ、 1 台のみで運用やメンテナンスを行う必要があります。
端末の台数が多い場合、運用がかなり大変です。
また、 Apple Remote Desktop が動作するには同一ネットワーク上である必要があるため、リモートの場合などのにおいて対応できない場合があります。

//footnote[macOS Server][Apple 社が提供する、macOS のサーバーアプリケーション（旧 OSX Server ）]
//footnote[MDM Command][MDM で提供されているワイプやロックを行うための命令の総称]
//footnote[Apple Remote Desktop][Apple 社が提供する、ネットワーク上の macOS 端末に対してさまざまな操作が可能なアプリケーション]

=== Microsoft Intune
Microsoft 社が提供している EMM サービスです。
Windows だけではなく、 macOS や iOS 、 Android などの Microsoft 社製品ではない OS を搭載した端末も管理が可能となります。

macOS において Microsoft Intune では、

  * インベントリ管理
  * 構成プロファイルの配布
  * MDM コマンドの送信

が可能となります。

しかし、 Microsoft の専門分野でないだけあり、できることがプロファイルマネージャよりも少ないです。
Office 365 または Microsoft 365@<fn>{Microsoft 365} にすでに加入しており、インベントリ登録のみを行いたい場合は追加の費用がかからないため有用であるかもしれません。

//footnote[Microsoft 365][Microsoft 社が自社で比較サイト（https://www.microsoft.com/ja-jp/microsoft-365/compare-all-microsoft-365-plans）を用意している]

=== Cisico Meraki System Manager
Cisico Meraki 社が提供している EMM サービスです。

Cisico Meraki System Manager では、

  * インベントリ管理
  * App Store の App の配布
  * 構成プロファイルの配布
  * インストール済み App 一覧
  * MDM コマンドの送信
  * 任意の Script の実行

が可能となります。

プロファイルマネージャとほぼ同等のことが可能です。
しかし、ライセンス費用がかなり高額であるという話を聞きました。

=== Jamf PRO
Jamf 社が提供している EMM サービスです。

Jamf PRO では、

  * インベントリ管理
  * App Store の App の配布
  * 構成プロファイルの配布
  * インストール済み App 一覧
  * MDM コマンドの送信
  * PKG ファイルの配布と実行
  * DMG ファイルのファイル構成をそのまま展開
  * 任意の Script の配布と実行

が可能となります。

プロファイルマネージャでできることはもちろん可能ですが、プロファイルマネージャと異なるのは「管理特権を所有するエージェント」が macOS 上で動作している点です。
このエージェントは、端末のインベントリ情報の収集はもちろん、ポリシーと呼ばれる単位で上記に記載したとおり、

  * PKG ファイルの配布と実行
  * DMG ファイルのファイル構成をそのまま展開
  * 任意の Script の配布と実行

が可能となります。
また、 Jmaf は新しいバージョンの OS に対してゼロデイサポートを謳っており@<fn>{jamf zero day}、対応がとても早いです。

2018 年現在、全世界で Jamf PRO が macOS の管理ツールのデファクトスタンダードとなっています。

以降は Jamf PRO を選択したことを前提に話を進めていきます。

//footnote[jamf zero day][What does zero-day support really mean?（https://www.jamf.com/blog/what-does-zero-day-support-really-mean/）]

== 端末管理の補助ツール

さきほど述べた Apple のソリューションおよび端末管理のサービスだけでは、どうしても実現できないこともあります。
そのため、それらでは対応できないことはサードパーティ製のツールで補完して行く必要があります。

=== NoMAD

macOS のログイン認証を Active Directory と一緒にしたいという要望は多く上がります。
確かに、パスワード複数覚えるのは面倒であるため、認証基盤と同じ物を使用したいと考えるのはごく自然です。

macOS では Active Directory と Bind がサポートされています。

もし macOS 上でモバイルアカウントを作成していた場合、パスワード変更時に、パスワードが同期されない問題がありました。
それを解決するために、 ADPassMon@<fn>{ADPassMon} というアプリケーションを使用することで解決ができていました。
しかし、この ADPassMon は macOS Sierra（10.12） からサポートされなくなってしまいました。

そこで、登場したのが NoMAD です。
NoMAD は アメリカの Orchard＆Grove 社が開発している、 macOS のログイン認証、主に Active Directory の連携周りをサポートするツールです。
OSS で公開されているコミュニティ版@<fn>{NoMAD OSS}も存在します。
また、有償版は macOS が提供するログイン画面ではなく、独自のログイン画面設置が可能です。
さらに SAML 連携も可能です。

元々、 macOS が提供されている Active Directory の Bind は不具合が多く、ログインがままならないこともありました。
この NoMAD を使用することで、スムーズに Active Directory と連携できるようになりました。

//footnote[ADPassMon][ADPassMon は以前 OSS として https://github.com/macmule/ADPassMo で開発が行われていた]
//footnote[NoMAD OSS][NoMAD は OSS として https://gitlab.com/Mactroll/NoMAD で開発が行われている]

====[column] Jamf Connect
Jamf 社は 2018 年 9 月に、 NoMAD を開発する Orchard＆Grove 社を買収しました。@<fn>{Jamf Acquires NoMAD}

その際、 Jamf 社は NoMAD において有償版で提供していたソリューションを $24/device/year にて、 Jamf Connect という名前で提供することを発表しました。
OSS 版に関しては引き続き OSS にて開発を進めて行くとのことです。@<fn>{Jamf Connect Q&A}

//footnote[Jamf Acquires NoMAD][2018年9月19日にJamfが公式にNoMADを買収したことを発表（https://www.jamf.com/resources/press-releases/jamf-acquires-nomad-the-leading-solution-for-streamlining-mac-authentication-and-account-management/）]
//footnote[Jamf Connect Q&A][Jamfが今後のNoMADのロードマップを発表（Jamf Connect Q&A https://www.jamf.com/blog/jamf-connect-qa/）]

=== Google Santa

Jamf PRO には Restricted Software という機能が存在します。
これはプロセス名に一致したアプリケーションを強制終了することができます。
しかし、プロセス名の指定は文字列の部分一致であり、プロセス名を変更されると、この機能を回避が可能です。

Google Santa は Google 社が OSS で開発している@<fn>{Google Santa}ブラックリストおよびホワイトリストベースのバイナリ起動制限ツールです。

  * 証明書ベース
  * SHA-256 ハッシュベース
  * 開発者ベース

での制限が可能となります。

また、Google Santa の設定をホスティングすることで、複数台の端末に設定を展開することも可能です。

macOS に Google Santa をインストールすると、 @<code>{santactl} コマンドが提供され、起動バイナリを監視するデーモンが動作します。

//footnote[Google Santa][https://github.com/google/santa で開発が進められている]

=== OpenBSM

macOS には OpenBSM と呼ばれるオープンソースで開発されている@<fn>{OpenBSM}イベント監査システムがデフォルトで入っています。
@<code>{/etc/security/audit_control} ファイルに、どのようなイベントをどれだけ取るのかなどの設定を記載できます。

特に重要なパラメータは、

  * @<code>{flags}
  ** ユーザに起因する監査イベントのクラスの定義
  ** 複数指定は @<code>{, （カンマ）}区切り
  * @<code>{naflags}
  ** ユーザに起因しない監査イベントのクラスの定義
  ** 複数指定は @<code>{, （カンマ）}区切り
  * @<code>{policy}
  ** フェイルストップ、パスの監査と引数など、さまざまな振る舞いを指定するグローバルな監査ポリシーフラグの定義
  ** 複数指定は @<code>{, （カンマ）}区切り

の 3 つです。
macOS で使用できるイベントクラスは @<table>{Event Class List}、ポリシーフラグは @<table>{Policy Flag List}のとおりです。

しかし、この audit 機能で全ての監査が可能となるわけではありません。
また、出力されるログファイルが扱いづらいのが難点です。

//footnote[OpenBSM][https://github.com/openbsm/openbsm で開発が進められている]

//table[Event Class List][audit_control のイベントクラス一覧]{
Code	Class	Description
-------------------------------------------------------------
0x00000000	no	クラスの無効化
0x00000001	fr	ファイル読み込み
0x00000002	fw	ファイル書き込み
0x00000004	fa	ファイル属性アクセス
0x00000008	fm	ファイル属性変更
0x00000010	fc	ファイル作成
0x00000020	fd	ファイル削除
0x00000040	cl	ファイルの close システムコール
0x00000080	pc	プロセスイベント
0x00000100	nt	ネットワークイベント
0x00000200	ip	IPC 操作
0x00000400	na	ユーザーが原因ではないイベント
0x00000800	ad	管理上の操作
0x00001000	lo	ログインおよびログアウトイベント
0x00002000	aa	認証および認可
0x00004000	ap	アプリケーションが定義するイベント
0x20000000	io	@<code>{ioctl} システムコール
0x40000000	ex	プログラムの実行
0x80000000	ot	デバイス割り当てや @<code>{memcntl()} などのその他
0xffffffff	all	全てのフラグ
//}

//tsize[10,80]
//table[Policy Flag List][audit_control のポリシーフラグ一覧]{
Flag	Description
-------------------------------------------------------------
cnt	イベントが監査されてなくてもプロセスが実行できるようにします。設定されていない場合、監査ストアの領域がなくなるとプロセスは中断されます。
ahlt	イベントを監査できない場合は、システムを停止します。
argv	@<code>{execve(2)} のコマンドラインの引数を監査します。
arge	@<code>{execve(2)} の環境変数を監査します。
//}

=== osquery

OpenBSM での監査はとても使いにくいものでした。

osquery は Facebook 社が OSS で開発を行っている@<fn>{osquery-footnote} SQL ベースの分析および監査フレームワークです。
macOS 以外にも Windows や Linux にも対応しています。

2018 年 10 月現在、 macOS においては 162 のスキーマが用意されています。

macOS にインストールすると、 @<code>{osqueryi} コマンドが提供され、クエリの実行をスケジューリングするデーモンが動作します。

実行したクエリの結果を Elasticsearch@<fn>{Elasticsearch} などのホストに収集することで、統合的に監査や監視が可能となるでしょう。

//footnote[osquery-footnote][https://osquery.io/ に概要が記載されており、 https://github.com/facebook/osquery で開発が進められている]
//footnote[Elasticsearch][Elastic 社が開発している OSS の全文検索エンジン]

== 終わりに

本章では、 macOS 端末の管理にはどういったツールや仕組みがあるかを説明しました。
運用時に実際どのようなことを行っているかについては、別の機会に事例を交えて紹介できればと思います。
