= FOLIO iOSアプリのウォークスルーの作り方　──動的な要素を無くし、静的で読みやすい構造を保つためのヒント──

アプリの内容をユーザーに伝えるために、アプリの初回起動時にウォークスルーを表示することがあります。
FOLIOのアプリでは、Android/iOSでそれぞれ異なるアニメーションを実装しています。

本章ではiOS版のアプリのウォークスルーを実現している仕組みについて、主にビューの構造について解説します。

また、（実際には採用されなかった）初期のナイーブな実装と、最終的に採用された実装の比較を通して、読みやすくバグを作りにくいビューのプログラミングテクニックをお伝えします。

ウォークスルーという動きをともなうUIの解説ですので、文章で読むだけでは理解しづらい部分があるかと思います。実際の挙動を理解するための補助として、また本章に書ききれないコードの確認のために、動作するサンプルコードをGitHub（@<href>{https://github.com/folio-sec/Folio-UI-Collection}）にて公開しています。

実際のプロジェクトを動かしながら読まれると、より理解が深まることと思います。

== ウォークスルー画面の挙動について

まずはじめに、本章で解説するウォークスルーの画面がどのように動作するものか説明します。

最初にアプリを起動すると、ウォークスルーとして次のような画面が表示されます。

//image[page1-0][1ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}

画面を横方向にスワイプすると、ページ単位でスクロールします。ページ数は全部で４ページです。
スクロールせずに常に表示されている要素として、

 * ウォークスルーをスキップするボタン
 * 現在ページ位置とページ数を示すコントロール

があります。

//image[page1-3][1〜2ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}
//image[page1-7][1〜2ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}

基本的には４ページぶんの画面をページ単位でスクロールできる・・・というものなのですが、１ページ目と２〜４ページ目で動きが異なります。２〜４ページ目にはウォークスルーのコンテンツとして、

 * 背景
 * テキストラベル
 * iPhoneの画面を模した枠
 * iPhoneの画面を模した枠内の画像

が表示されます。

この「iPhoneの画面を模した枠」の中の画像は、枠の幅でさらにページ単位でスクロールします。これは全体のスクロールと同期しますが、ページの幅が異なるため、いわゆるパララックス効果をともなうスクロールになります。

さらにこの「iPhoneの画面を模した枠」は１ページ目から２ページ目の間のみスクロールに追随しますが（@<img>{page1-3}、@<img>{page1-7}）、２ページ目以降は固定されます（@<img>{page2-3}、@<img>{page2-7}、@<img>{page3-0}）。

//image[page2-0][2ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}
//image[page2-3][2〜3ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}
//image[page2-7][2〜3ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}
//image[page3-0][3ページ目（画像・データ等は技術説明のためのサンプルです。）][scale=0.3]{
//}

この１ページ目と２ページ目以降でスクロールの動作が変わるが、全体としてはシームレスなスクロールになっているという背反した振る舞いを両立しなければならないということが問題になります。（この動きは文章と画像だけで伝えるのは難しいのでぜひサンプルコードを動作させて確認してみてください）

== 初期のナイーブな実装

まず初期の実装における構造を解説します。
最初の実装では外側のスクロールビュー（Outer Scroll View）と内側のスクロールビュー（Inner Scroll View）の２つのスクロールビューを組み合わせて実装されていました。

//image[initial-view-structure][初期のビュー構造][scale=0.7]{
//}

内側のスクロールビューは@<code>{isUserInteractionEnabled}を@<code>{false}に設定し、タッチに反応しないようにして、外側のスクロールビューの動きに同期するようにプログラムから操作するようにします。

処理としてはまず、外側のスクロールビューがスクロールされると、デリゲートメソッドの@<code>{scrollViewDidScroll(_:)}がコールバックとして呼ばれるので、そのタイミングで内側のスクロールビューのスクロール位置（@<code>{contentOffset}）を操作します。

ここまではオーソドックスなパララックスの実装です。

さて問題となる１ページ目から２ページ目のスクロールはパララックス効果をともなったスクロールをせずに、なおかつ、内側のスクロールビューのコンテンツが外側のスクロールビューのスクロールに追随して画面内にあくまでの２ページ目のコンテンツとして現れる必要があります。

また、２ページ目以降は外側のスクロールビューにつれて内側のスクロールビューが動いてしまってはいけません。

この実装では２ページ目以降で外側のスクロールビューの動きに追随してしまわないように、実際にはビューの階層関係としては２つのスクロールビューはスーパービューとサブビューの関係（内側のスクロールビューが外側のスクロールビューの上に載っている）ではなく、互いに独立して配置されています。

//image[initial-view-hierarchy][初期実装におけるビューの階層関係][scale=0.7]{
//}

外側のスクロールビューにつれて内側のスクロールビューが動いてしまわないことはそれで実現できますが、そのままでは１ページ目から２ページ目の動きが実現できません。

そこで１ページ目から２ページ目のスクロールという条件のとき、内側のスクロールビューのCGAffineTransformに平行移動を設定し、あたかも外側のスクロールビューのページがスクロールされるとともに動くように見せかけるという処理を書きます。

//list[UIScrollViewDelegate][スクロールの処理]{
extension WalkthroughViewController: UIScrollViewDelegate {
    func scrollViewDidScroll(_ scrollView: UIScrollView) {
        guard scrollView == mainScrollView else { return }
        if pageControl.currentPage == 0 && scrollView.contentOffset.x < 0 {
            scrollView.contentOffset = CGPoint(x: 0, y: 0)
        } else if pageControl.currentPage == 3 &&
        　　　　　　scrollView.contentOffset.x > scrollView.bounds.size.width * 3 {
            scrollView.contentOffset =
                CGPoint(x: scrollView.bounds.size.width * 3, y: 0)
        } else {
            // ここで筐体のスクリーンを動かす
            if scrollView.contentOffset.x > UIScreen.main.bounds.width {
                // 2ページ目から筐体スクリーンはスライドする
                subScrollView.contentOffset.x =
                    mainScrollView.contentOffset.x /
                        (self.view.frame.maxX / mockScreenSizeWidth)
            }
        }
        // 筐体とsubScrollViewを2ページ目にくるまではCGAffineTransformにて移動させ、2ページ目からは停止
        if scrollView.contentOffset.x <= UIScreen.main.bounds.width {
            walkthroughMock.transform =
                CGAffineTransform(translationX: -scrollView.contentOffset.x, y: 0)
            subScrollView.transform =
                CGAffineTransform(translationX: -scrollView.contentOffset.x, y: 0)
        }
    }

    func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
        guard fmod(mainScrollView.contentOffset.x, mainScrollView.frame.maxX) == 0
            else { return }
        pageControl.currentPage =
            Int(mainScrollView.contentOffset.x / mainScrollView.frame.maxX)
        if pageControl.currentPage > 0 &&
           -walkthroughMock.transform.tx < UIScreen.main.bounds.width / 2 {
            // スクロールの判定が追いつかない速度でスクロールされた場合の補正
            walkthroughMock.transform =
                CGAffineTransform(translationX: UIScreen.main.bounds.width, y: 0)
            subScrollView.transform =
                CGAffineTransform(translationX: UIScreen.main.bounds.width, y: 0)
        }
    }
}
//}

まとめると、初期のウォークスルーの実装はスクロールビューのページング機能を利用し、ページサイズの異なる２種類のスクロールを表現するために２つのスクロールビューを組み合わせて実現されています。
２つのスクロールビューは互いに独立ですが、ページ幅の比率によって同期するコードを記述します。パララックス効果をともなうスクロールが実現できます。

ただし、１ページ目から２ページ目のスクロールについては内側のビューが外側のビューに「載って」いるような表現をしなければならないので、２ページ目以降のスクロールの違いをプログラム中で動的に条件分岐をして挙動を変えています。

== 問題の発見とリファクタリング

ここで私が問題に感じたことは、この動的に行なっている処理です。特に１ページ目と２ページ目以降の挙動の違いをそのままコードで表現しているために、見た目の動きとおりのかなり読みにくいコードになってしまっています。

ビューのプログラミングに限りませんが、特にビューにおいてはできるだけ静的な構造を定義し、動的な振る舞いを無くしていくことが、読みやすくバグを作りにくい高い品質のコードを書くために重要です。

複雑な動きやアニメーションをともなう見た目だからといって、そのまま複雑なコードで表現する必要はありません。問題を小さく分割しシンプルな構造として表現できないかどうか考えてみましょう。

次項ではこの動きをよりシンプルで静的な構造として表現する方法を説明します。

== リファクタリング後の実装

最終的に採用された実装ではビューの構造は次のようになっています。

//image[final-view-structure][リファクタリングしたビュー構造][scale=0.7]{
//}

パララックス効果を実現する外側（Scroll View A）と内側（Scroll View C）の２つのスクロールビューを使用するところは同じですが、１ページ目と２ページ目のスクロールを実現するためにもう１つスクロールビュー（Scroll View B）を使用します。

この３つめのスクロールビューによって、前項で解説した動的な処理が必要となる問題を解決します。

３つのスクロールビューのうち、タッチイベントに反応するものは以前の実装と同様に一番外側のスクロールビュー（Scroll View A）だけです。それ以外の２つのスクロールビューの@<code>{isUserInteractionEnabled}を@<code>{false}に設定し、タッチに反応しないようにします。

そしてスクロールビューAとスクロールビューBのスクロールを同期させます。
ここで、スクロールビューBのスクロール範囲は２ページぶんしかないため、同期する２ページ目以降のスクロールが止まります。

//list[scroll-sync-ab][スクロールビューAとBのスクロールを同期]{
scrollViewA.rx.contentOffset
    .subscribe(onNext: {
        // Scroll View Aと1〜2ページの間で連動する
        scrollViewB.contentOffset.x = min(scrollViewB.bounds.width, $0.x)
        ...
    })
    .disposed(by: disposeBag)
//}

さらにスクロールビューAと内側のスクロールビューCのスクロールをページ幅の比率で同期させて、パララックス効果を実現します。

//list[scroll-sync-ac][スクロールビューAとBのスクロールを同期]{
scrollViewA.rx.contentOffset
    .subscribe(onNext: {
        ...
        // Scroll View Aとページ幅の比率で連動する
        let factor = scrollViewC.bounds.width / scrollViewB.bounds.width
        let offsetX = max(0, min(($0.x - scrollViewB.bounds.width) * factor,
                                  scrollViewC.contentSize.width -
                                  scrollViewC.bounds.width))
        scrollViewC.contentOffset.x = offsetX
    })
    .disposed(by: disposeBag)
//}

これだけです。最初の実装と比較すると、格段にシンプルな処理で実現できました。

このように同じ方向にスクロールする幅の異なるスクロールビューを複数用いて、ビューの構造を定義することにより、静的に複雑なスクロールの挙動を定義できました。

//image[final-view-structure-code][内側のスクロールビューを外側のスクロールと連動させるだけ][scale=0.7]{
//}

１ページ目だったら、２ページ目以降だったら、という条件分岐はなくなり、代わりにそれぞれのスクロールビューの横方向の@<code>{contentOffset}を自身の@<code>{contentSize}の範囲で同期する処理に置き換わりました。そこにページ幅の比率をかけるとパララックス効果が得られます。

というわけで、スクロールに関する最終的なコードの全体は次のようになります。

//list[all][最終的なコードの全体]{
scrollViewA.rx.contentOffset
    .subscribe(onNext: {
        // Scroll View Aと1〜2ページの間で連動する
        scrollViewB.contentOffset.x = min($0.x, scrollViewB.bounds.width)

        // Scroll View Aとページ幅の比率で連動する
        let factor = scrollViewC.bounds.width / scrollViewB.bounds.width
        let offsetX = max(0, min(($0.x - scrollViewB.bounds.width) * factor,
                                  scrollViewC.contentSize.width -
                                  scrollViewC.bounds.width))
        scrollViewC.contentOffset.x = offsetX
    })
    .disposed(by: disposeBag)
//}

プロダクトコードでは両端のバウンスの見た目を整える処理など、もう少し余分なコードがありますが、「条件分岐を（できるだけ）無くし、静的に定義する」というコンセプトを守って書かれています。

== 終わりに

以上、簡単ですがFOLIO iOSアプリのウォークスルーのスクロールを実現する仕組みを解説しました。また初期のナイーブな実装と最終的に採用された実装の比較を通して、よりシンプルな構造で見通しのよいコードを書くための方法に触れました。

いかがしょうか？最終的なコードは実際の見た目から受ける印象よりもかなりシンプルな構造とコードで実現できていることがわかります。

このように、動的な条件分岐を減らし、静的な定義だけで動く部分を増やしていくことにより、読みやすくバグの少ないコードになります。これは複雑な問題を小さな単位に分割し、個々の処理をシンプルに保つことに繋がり、高い品質の改善を継続する原動力となります。

この文章が、読者の方のコードの改善につながればとても嬉しく思います。
