= Introduction to Open Policy Agent
本章では2017年12月、CNCFにsandboxとして認定されたOpen Policy Agent（OPA）の概要について紹介します。
とくにRBACなどで苦労されているシステム管理者にとって知っておいて損がないOSSかと思います。
Terrafrom Enterpriseで利用されているSentinelや、GCPのContext-aware policyなども含む
Zero Trust Networkな環境のなかで「ポリシーエンジン」と呼ばれるコンポーネントがありますが、
OPAはOSSとしてそこに属します。

最新の情報は@<href>{https://openpolicyagent.org}をご覧ください。

===[column] 「ポリシー」の定義
ポリシーは情報セキュリティにおいては上位規定とされていますが、本章およびOPAの文脈では「システムの動作を統制するルール群」になります。

== OPAとは
OPAは「オーパ」と読み、管理者のためのクラウドネイティブな環境におけるアクセス制御を
ポリシーベースで実現するOSSです。

== NWからRBACへ
電子的なアクセス制御は学術的なネットワークからスタートしました。
ネットワークベースのアクセス制御はざっくりしてはいるものの、ある程度は身元が保証されたユーザーしか存在しなかったため問題はありませんでした。
しかし、ネットワークがワールドワイドになっていくとともにその前提は崩れていきました。
それでも2010年ぐらいまでであれば企業レベルのネットワークでの制御が可能でしたが、内部ではそうはいきません。
特にクラウドを利用することへの抵抗感が減った今、ネットワーク制御をしたくてもできなくなってきているのが現状ではないでしょうか。
また、企業としてはガバナンスをする意味で、適切に職権分離をしていく必要が以前からありました。
そういった背景から、Role Based Access Control（RBAC）が生まれました。最近だと、より細かい情報から制御できるようにAttribute Based Access Controlなども
登場していますが、ユーザーに割り当てられた職務を属性として考えるのであればコンセプトは同じです。
こういった機能は、Active DirectoryやLdapやLinuxなオンプレミスなサーバー、OktaやOneLoginなどのようなクラウド上のIdentity Providerにも実装されています。

== RBACのつらみ
では、これによりシステム管理者が楽になったかというとそうではありません。
たとえば新しい機能を追加することを考えます。
小規模で開発するために、今までバックエンドエンジニアであったメンバーがインフラにも携わるようになったり、
その引き継ぎに４ヶ月かけても終わらず、結局権限が広がり続けることがあり、組織図とRBACがマッチしない状況になりがちです。
こういったRBACを適切に運用していくこと自体もつらいのですが、それと一緒にケアしなければいけない見落としがちな問題が３つあります。

 1. 新しく作成・変更されたルールの適用
 2. 運用パターンの増加
 3. 運用に関する知見の共有

==== 新しく作成・変更されたルールの適用がつらい

まず、よくあるマスターとノードの構成で、RBACを適用させる方法を考えます。

//image[master-node][マスターとノード構成][scale=0.9]{
//}

設定ファイル（@<list>{rbac-example}）は、何かしらのサーバーへ、何かしらのユーザーとしてログインをするときのRBAC制御を記述したものです。
次の例は、"outsources"ロールに割り当てられたユーザーは"outsource"ユーザーとして"dev"環境にのみログインできることを記述したyamlファイルです。
ユーザーがノードにログインしようとすると、ノードはユーザーがログイン可能かチェックするリクエストをマスターに飛ばします。マスターはそれを設定をもとに決定しノードに決定結果を通知します。

//listnum[rbac-example][rbac-for-outsource.yaml]{
kind: role
version: v3
metadata:
  name: outsources
spec:
  allow:
    logins:
    - outsource
    node_labels:
      env:
      - dev
 deny: {}
//}

"stg"環境にもログインできるようにしたい場合、11行目に"stg"と書き加え、マスターに適用するだけです。
では、FirewallやAWSアカウントなどで区切られたmaster-node構成が複数ある場合はどうでしょうか？
当然、全masterに向けて設定ファイルを作成・変更して、それぞれに適用せねばなりません。
設定ファイルもバラバラとおかれ、複数回同じ作業が発生します。
このような状況は、運用・保守の観点から理想とは言い難いです。

また、全ての環境においてsandbox環境のような検証環境があるわけではありません。クラウドサービスなどは基本一発勝負となり、サービスの仕様次第ではもし失敗すると誰もログインできなくなるといった
いわゆる@<strong>{足を撃ち抜いた}状態に陥る可能性があります@<fn>{newrelic}。

//image[multi-master-node][複数からなるマスターノード構成][scale=0.9]{
//}

//footnote[newrelic][筆者にはNew Relicというサービスで実際に撃ち抜いた経験があります。サポートに大急ぎで問い合わせ、ことなきを得ました。]

==== 運用パターンの増加がつらい
さきほどの「足を撃ち抜いた」例としてNew Relicをあげました。New RelicにはNew Relicの仕様があり、それを理解してなかったがゆえに起きたことです。
しかし、New Relicないしは監視サービスだからこの問題が発生したかというと、そういうわけではありません。
IdP, Active Directory, AWS, GCP, Terraform, サービス管理画面...など、企業活動で必要なサービスの種類だけ、同じインシデントが発生する可能性があります。
しかし、前述したとおり企業のガバナンスとして適切な職権分離は必要であり、RBACをしないという選択肢は基本的にありません。また、マネージド・サービスにおけるライセンスの問題もあるため、十把一絡げに全員に同じ権限を与えるわけにもいきません（Datadog高いですね）。

//image[multi-systems][複数のシステム][scale=0.9]{
//}

このようにいろいろなシステムのRBAC仕様を理解しつつ、同一のポリシーを適用していくところにつらみがあるわけです。
そしてポリシーが変わるたびに全システムへミスなく適用しなければなりません。ここのつらみについてはさきほど説明したとおりですが、よりしんどさがわかってもらえるかと思います。

==== 運用に関する知見の共有がつらい
「運用パターンの増加」と「ポリシー作成・変更」のつらみを紹介しましたが、これを組織として運用しなければなりません。
したがってチーム内でその知見を共有する必要があるわけですが、まず全体のポリシー設計そして各システム・言語の仕様を共有し、スキルをできるだけ平準化する必要があります。
これをシステムのパターン数（N)と環境数（N）と人数（N）に実施しなければならない工数をご想像ください。
さらにいうと、大多数のシステムはRBAC設定を管理者にならなければ運用できませんが、逆に考えれば、RBACの運用のみであるにもかかわらず、過分な権限のように思えます。
適切なアクセス制御をしたいのに、過剰な権限を渡す矛盾を孕んでいるわけです。

== Policy Engineの登場

紹介の都合上、前節では「Configで記述したポリシーの運用」->「人間が共有するポリシーの運用」の順で、ポリシーの適用方法を紹介しました。
しかし、本来歴史として先にあったのは人間味あふれるマニュアル運用で、そこからConfigベースのポリシー適用になっていったという経緯があります。
Policy Engineはそれらが発展してできた、新しいポリシー適用方法です @<fn>{what-is-policy}。

//image[history-of-policy][ポリシー適用の歴史][scale=0.9]{
//}

OPAのドキュメント@<fn>{opa-introduction}によると、Policy Engineの役割は次のとおり定義されています。

//quote{
decouples the policy implementation from the business logic so that administrators can define policy without changing the application while still keeping up with the size, complexity, and dynamic nature of modern applications
//}

今までシステム・サービスおよびそこに適用するポリシー（とその設定ファイルなど）は一体となっているのが一般的でした。
どのルールを適用するかの決定（@<b>{Decision}）と、その決定をシステム・サービスに適用する（@<b>{Enforcement}）レイヤーが同一箇所にあったということです。
しかし先述したとおり、それでは仕様がそれぞれのシステム・サービスで異なる場合や適用する対象のシステムが増えたときにつらみが発生します。
かといって、システムがどんどん増えていく中、ドキュメントやマニュアルでの共有・更新を行うのもなかなか厳しいものがあります。
OPAを始めとしたPolicy Engineの登場によって、@<b>{Enforcement}と @<b>{Decision} を分離できるようになりました。
ですが、システムやサービスらがもともと持っていた@<b>{Decision}の役割を引き継ぐ形になります。

//image[opa-concept][EnforcementとDecisonレイヤーの分離][scale=0.9]{
//}


このように@<b>{Decision}レイヤーをシステム・サービスから分離することで、動的に変化するそれらへの影響を最小限に抑えることができます。

また、OPAはJSONフォーマットでやりとりするRestfulなHTTP APIエンドポイントとして稼働させることもできます。システム・サービスがhttpクライアントとしてクエリを投げられるのであれば、
ポリシーを決定するレイヤーと言語をOPAに統一できます。もちろんOPAを拡張することでよりフレキシブルなポリシーの適用と運用が可能です。

//footnote[what-is-policy][@<href>{https://blog.openpolicyagent.org/what-is-policy-part-one-enforcement-bad8ea8eb35c}]
//footnote[opa-introduction][@<href>{https://www.openpolicyagent.org/docs/}]

== OPAの動き方
本節では、OPAの中身を動作させながら見ていきます。まず、OPAは大別して２つの要素からなっています。
@<b>{Policy（もしくはRule）}と@<b>{Document（もしくはData）}です。これらを紹介したのちにOPAを動かしながらどのように動くのか見ていきましょう。

=== Policy
OPAのPolicyは@<strong>{Rego}と呼ばれる宣言的言語で記述されます。
それぞれのRegoファイルにはシステムやサービスの期待された状態をルールとして記述します。このルールの集合を@<strong>{Policy module}と呼びます。
詳しい文法やテスト方法は公式のHow Do I Write Policies?@<fn>{how-do-I-write-policies}やHow Do I Test Policies?@<fn>{how-do-I-test-policies}をご覧ください。
各Regoファイルは"package"から始まるユニークな宣言をする必要があります。
ここでの宣言はPolicy Moduleおよび各ルールへのパスとなります。

//footnote[how-do-I-write-policies][https://www.openpolicyagent.org/docs/how-do-i-write-policies.html]
//footnote[how-do-I-test-policies][https://www.openpolicyagent.org/docs/how-do-i-test-policies.html]


//list[opa-server-policy][サーバーポリシー]{
$ cat server-rule.rego
package opa.example

import data.servers
import data.networks
import data.ports

public_servers[s] {
    s = servers[_]
    s.ports[_] = ports[i].id
    ports[i].networks[_] = networks[j].id
    networks[j].public = true
}

violations[server] {
    server = servers[_]
    server.protocols[_] = "http"
    public_s
//}

Regoファイルは、OPAのポリシーAPIによって反映されます。
次の例では、さきほど作ったRegoファイルを@<code>{ex-server}モジュールとしてアップロードしています。
なおex-serverは、あくまでもファイル管理のためのポリシー識別子であって、それ以外に利用されることはありません。

//list[opa-policy-upload][OPAにPoicy作成]{
$ curl -X PUT --data-binary @server-rule.rego localhost:8181/v1/policies/ex-servers
//}

=== Document
DocumentはJSONフォーマットのデータで、Base DocumentとVirtual Documentの２種類があります。Base Documentは基本的にインメモリに保管されるスタティックなデータですが、堅牢性を重視したい場合ディスクに永続化させることも可能です。
@<b>{Enforcement}レイヤーであるシステムやサービス、そしてそれらのユーザーは自分のcurrent stateをRestful API経由でDocumentとしてアップロードします。
たとえばある環境のサーバー群をDocumentにしたい場合、次のクエリを実行します。

//list[opa-server][サーバー群のDocument]{
$ curl -X PUT --data-binary @data.json localhost:8181/v1/data/servers

{
    "servers": [
        {"id": "s1", "name": "app", "protocols": ["https", "ssh"],
          "ports": ["p1", "p3"]},
        {"id": "s2", "name": "db", "protocols": ["mysql"],
          "ports": ["p1", "p2"]},
        {"id": "s3", "name": "cache", "protocols": ["memcache"],
          "ports": ["p1", "p2"]},
        {"id": "s4", "name": "dev", "protocols": ["http"],
          "ports": ["p1", "p3"]}
    ],
    "networks": [
        {"id": "n1", "public": false},
        {"id": "n2", "public": false},
        {"id": "n3", "public": true}
    ],
    "ports": [
        {"id": "p1", "networks": ["n1"]},
        {"id": "p2", "networks": ["n2"]},
        {"id": "p3", "networks": ["n3"]}
    ]
}
//}

ここでは４つのサーバーがありますが、このうち@<code>{n3}が@<code>{public}になっており、@<code>{p3}が属する@<code>{s1}と@<code>{s4}がインターネットに公開されています。

Virtual DocumentはRuleによる評価の結果を表すデータです。
Virtual Documentはユーザーが新たにルールセット（Policy Module）を定義・更新したり、クエリを発行したり、関連するBase Documentが作成・更新されたときに計算・生成されます。

ここまで行うと、全体のDocumentは次の構成になります。

//image[server-documents][Document全体像][scale=0.9]{
//}

ここでは青がBase Document、緑がVirtual Document、オレンジがRegoファイルを示します。
図のように、サービス・システム・ユーザーが直接アップロードしたデータはdata配下に保存され、
Regoファイルの適用によって生成されたVirtual Documentはパッケージ（opa.example）配下に保存されることがわかります。
これら２種類のファイル群のrootは'data'です。Base DocumentにもVirtual DocumentもルートをdataとするDocument内に取り込まれているため、階層的にアクセスできます。

たとえば、どのサーバーが公開されていてかつ平文httpアクセスをしているか知りたい場合は、Policy Moduleのpackage名を指定したクエリを実行すれば取得できます。

//list[violated-servers][ポリシーに違反したサーバーの取得]{
$ curl -X GET http://localhost:8080/v1/data/opa/examples/violations | jq .
{
  "result": [
    {
      "id": "s4",
      "name": "dev",
      "protocols": [
        "http"
      ],
      "ports": [
        "p1",
        "p2"
      ]
    }
  ]
}
//}

このようにRegoとDocumentの組み合わせにより、さまざまな制御をさまざまなシステムで適用できます。今回はシンプルなケースを紹介しましたが、OPAのドキュメントではDocker上の認可、SSH・sudoの認可、k8sの認可など、幅広く応用例が紹介されています。
Istio向けのプラグインも出ており、OPAも最新のインフラ環境にキャッチアップしていこうという気概がよく伺えます。

== 終わりに
本章ではOPAを紹介しました。システム管理者はさまざまなシステムのアクセス制御をしていかなければなりません。
OPAはその制御をRestful APIベースで一箇所に集約し、Regoという宣言的な言語によって、今までの運用・保守のつらみを軽減してくれる候補として期待できます。
サービス環境への転用はパフォーマンスの観点からまだ難しいと思いますが、社内環境には積極的に適用していきたいと筆者は考えています。
