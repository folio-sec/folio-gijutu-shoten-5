= FOLIO Androidアプリのウォークスルー解説
アプリの内容をユーザーに伝えるために、アプリの起動時にウォークスルーを表示することがあります。
FOLIOのアプリでは、Android/iOSでそれぞれ異なるアニメーションを実装しています。
本章ではAndroid版のアプリのウォークスルーの実装方法について解説します。
なお、サンプルコードについてはGitHub（@<href>{https://github.com/magiepooh/walk-through-sample}）へアップロードしています。
本章には載せきれないコードが多くあったため、GitHub上の実際のコードを見ながら読むことをお勧めします。
また、アニメーションの話なので、実際に動かしてみるとイメージしやすいと思います。

== ウォークスルーがなぜ必要か
ウォークスルーとはユーザーに対してアプリの特徴や使い方をざっくりと伝えるものです。
複雑なアプリではユーザーに全体像を理解してもらうことにより、スムーズにアプリを触ってもらうことができます。

一方で、ユーザーにとっては必ずしも見たいものではないということです。
そのため、ページ数を制限し、見たくなるようにインタラクションや仕組みを意識することが重要です。

== Android版のウォークスルー
//image[page1][1ページ目][scale=0.3]{
//}
//image[page1-2][1ページ目][scale=0.3]{
//}
//image[page1-3][1ページ目][scale=0.3]{
//}
//image[page1-4][1ページ目][scale=0.3]{
  背景完了
//}
//image[page2][2ページ目][scale=0.3]{
//}
本章で解説するウォークスルーのインタラクションについて説明します。
１ページ目であるロゴが表示される画面があります（@<img>{page1}）。
この画面を構成するのは次の4つです。これら4つがユーザーのインタラクションによって、アニメーションします。

 * 背景
 * 背景のアイコン
 * タイトル
 * 半透明の四角（実際のウォークスルーでは端末の画像なので、端末画像と呼ぶことにします）

ここからウォークスルーがスタートします。

２ページ目へ遷移する際、端末画像が回転しながら入ってきます（@<img>{page1-2}、@<img>{page1-3}）。
また、タイトルと背景のアイコンがパララックス（視差効果）を伴い、左方向へ移動します。
さらに後ろにある1枚塗りの背景がパララックスを伴い右側から左側へ描画されていきます。

引き続き動かすと端末画像などの2ページ目のもの入ってきますが、
2ページ目の背景の画像など固定される前に、2ページ目の背景のが先にすべて描画されます（@<img>{page1-4}）。

そのあとに、２ページ目の端末画像やタイトルやアイコンが適切な位置に固定されます（@<img>{page2}）。

== 実装
本節では実際の実装方法をコードを交えて説明します。

=== 実装方法の検討
ページ切り替えで思いつくコンポーネントといえばViewPagerです。
これを使って実装していきたいと思います。
単純にViewPager内の各々のViewに4つの要素を描画してよいのでしょうか？

ViewPagerはPagerAdapterによって生成されたViewが順番に並んでいます。
生成された各々のViewの間にマージンを設定できます。
そのマージンに負の値を設定すると重なります。
このとき、Viewは順番に生成されて重なっているため、片方が重なって表示されます（@<img>{viewpager-layer}）。

//image[viewpager-layer][ViewPagerの構造][scale=0.3]{
//}

背景と背景のアイコンについてはViewPagerの動きとは独立したものになっているため、
ViewPagerの各Viewに描画するのは大変そうです。
タイトルと端末画像についてはViewPagerと同じ動きになっているため、ViewPagerの各Viewに描画してもよさそうです。
しかし、FOLIOのAndroidアプリではViewPager内に描画することをやめました。
ViewPagerの移動スピードと各View内での移動スピードの合計したものが、ユーザーに見える移動スピードになります。
複数の状態を把握しないといけないため、デザイナーが考えた速度を簡単に作り出せないのです（作り出したとしても調整が大変です）。

では、まったくViewPagerを使わないという選択をするかというとそうではありません。
少なくとも、Viewの切り替え時にどの程度スクロールしているかを判断するために、@<code>{ViewPager.OnPageChangeListener}は使えそうです。

これらのことを踏まえ、@<code>{ViewPager.OnPageChangeListener}を使いたいのでViewPagerを使うが、
各Viewの中には何も描画しない。
端末画像やタイトル、背景、背景のアイコンはViewPager外に配置し、
このコールバックを用いて、これらのViewを操作するという選択をしました。

=== コールバックを一般化する
ページのスクロールを検知するために、@<code>{ViewPager.OnPageChangeListener#onPageScrolled}を使いましょう。
ここで使う引数は @<code>{position}, @<code>{positionOffset}の2つです。
positionは0から始まる現在表示されているページのポジションです。
positionOffsetは現在のページからのスクロール量のオフセットです。
スクロール量に応じて[0, 1)の値を取ります。（0以上1未満の値）

各ページの端末画像やタイトルは1, 0, -1の3つの状態を取るように設計したいと思います。
1は画面外の右側にある状態です。0は画面中央に固定されている状態。-1は画面外の左側にある状態です。
言い換えるなら、1はスタート、-1はゴールの状態です。0はそのページが選択されている状態です。

1ページ目の端末画像が0の状態のときに、2ページ目・3ページ目の端末画像は1でなければいけません。
この状態から1ページ目を左方向に動かし始めると、
1ページ目は-1へ、2ページ目は0へ近づき、3ページ目は1のまま固定されてほしいのです。

 * viewPosition: 端末画像が表示されるべきページ。1ページ目のViewは0,2ページ目のViewは1が指定されている。
 * pagerPosition: @<code>{ViewPager.OnPageChangeListener#onPageScrolled}で渡される現在の選択されているポジション。
 * positionOffset: スクロール量。[0,1)の値をとる。

これらの挙動を実現するコードを@<list>{view-pager-behavior}に示します。

//list[view-pager-behavior][ViewPagerBehavior][kotlin]{
interface ViewPagerBehavior {
  val onPageOffsetChanged: (Float) -> Unit

  /** [ViewPager.onPageScrolled]と繋がってView用のoffsetを計算する */
  fun onPageScrolled(viewPosition: Int): (Int, Float, Int) -> Unit =
    { pagerPosition, positionOffset, _ ->

      // offsetの計算を行う
      val offset: Float = viewPosition - pagerPosition - positionOffset
      // 画面外の部分に関しては最大の値に固定する
      onPageOffsetChanged.invoke(when {
        offset >= 1f -> 1f
        offset <= -1f -> -1f
        else -> offset
      })
    }
}
//}

このコードのoffsetは移動量を意味します。
具体的にどのような値が入るか見てみましょう。
1ページ目が選択されていて（@<code>{pagerPosition}が0）、
左に0.1だけスクロールした（@<code>{positionOffset}が0.1）場合を次に示します。

 * 1ページ目（viewPositionが0）のoffset = 0 - 0 - 0.1 = -0.1
 * 2ページ目（viewPositionが1）のoffset = 1 - 0 - 0.1 = 0.9
 * 3ページ目（viewPositionが2）のoffset = 2 - 0 - 0.1 = 1.9

1ページ目と2ページ目に関しては正しそうです。
3ページ目に関しては動いてほしくないため、1を上回る場合は1にしてしまいます。-1を下回る場合も-1に固定します。

次に2ページ目が選択されていて（@<code>{pagerPosition}が0）、
右方向に0.1だけスクロールした（@<code>{positionOffset}が1 - 0.1 = 0.9）場合を次に示します。
ここで@<code>{pagerPosition}が0になるのは1ページ目に戻ろうとしているからです。

 * 1ページ目（viewPositionが0）のoffset = 0 - 0 - 0.9 = -0.9
 * 2ページ目（viewPositionが1）のoffset = 1 - 0 - 0.9 = 0.1
 * 3ページ目（viewPositionが2）のoffset = 2 - 0 - 0.9 = 1.1

これらの計算結果であるoffsetを用いて、@<code>{onPageScrolled}のたびに移動量を決めてあげればよいのです。

=== 各数値の計算と実装方法
例として、端末画像の回転を取り上げます。
さきほど説明したoffsetが1のときは端末画像は画面外の右側に配置されていて、傾いている状態です（ @<img>{rotation}）。

このときの傾きの最大の角度をDEVICE_MAX_ROTATIONという定数とします。
中央からDEVICE_MAX_ROTATIONだけ右に傾いている状態です。

offsetが0のときは画面中央にまっすぐ表示されます。このとき、θは0になります。

offsetが-1のときは画面外の左側に配置されていて、傾いている状態です。中央から計算するとθだけ左に傾けます。

//image[rotation][端末画像の傾き][scale=0.25]{
//}

//list[onPageOffsetChanged][DeviceView#onPageOffsetChangedの中の計算]{
override val onPageOffsetChanged: (Float) -> Unit = { offset ->
  if (!isLaidOut && offset != 0f) {
    visibility = View.INVISIBLE
  } else {
    // ここで各要素の移動量の計算を行う
    if (visibility != View.VISIBLE) visibility = View.VISIBLE
    binding.title.translationX =
      titleMaxTranslationX * (offset / PARALLAX_DIVISOR_TITLE)
    binding.device.translationX =
      deviceMaxTranslationX * (offset / PARALLAX_DIVISOR_DEVICE)
    binding.device.rotation =
      DEVICE_MAX_ROTATION * (offset / PARALLAX_DIVISOR_DEVICE)
    binding.device.translationY = Math.abs(
            deviceMaxTranslationY * (offset / PARALLAX_DIVISOR_DEVICE))
  }
}
//}

端末画像の傾きをθとすると、@<code>{θ = DEVICE_MAX_ROTATION * offset}になります。
実際のコードは@<code>{DeviceView}クラスを参照してください。
よく見ると、@<code>{θ = DEVICE_MAX_ROTATION * (offset / PARALLAX_DIVISOR_DEVICE)}となっていて、offsetを定数で割っています。
これはパララックスの強度を表しています。@<code>{PARALLAX_DIVISOR_DEVICE}の値を変えて実行してみるとわかりやすいです。

このθの値ですが、画面外で固定されているものに対しては常に1または-1がコールバックで渡される仕組みになります。
しかし、このコールバックで行っている計算量はそんなに多いものではないため無視してもよいです。
また、Androidでtranslationやrotationを設定する場合、同じ値だったら無視されるように実装されているため、
描画の負荷にもなりません。（ココ重要）

ここでは端末画像の回転だけを見ましたが、タイトルに関しても同じように動かすことが可能です。

また、前の節で説明したviewPositionについてはViewPagerにAdapterを設定する際に、設定しています。（@<list>{register-callback}）

//list[register-callback][コールバックの登録]{
binding.viewpager.run {
  adapter = WalkThroughPagerAdapter()
  // 端末画像に関してはそれぞれのページに自身の位置を設定している。
  addOnPageChangeListener(onPageScrolled =
    device1View.onPageScrolled(WalkThroughPage.DEVICE_1.ordinal))
  addOnPageChangeListener(onPageScrolled =
    device2View.onPageScrolled(WalkThroughPage.DEVICE_2.ordinal))
  addOnPageChangeListener(onPageScrolled =
    device3View.onPageScrolled(WalkThroughPage.DEVICE_3.ordinal))
  addOnPageChangeListener(onPageScrolled = binding.backgroundView.onPageScrolled)
  addOnPageChangeListener(onPageScrolled = binding.iconsLayout.onPageScrolled)
}
//}

背景のアイコンの移動に関しても基本的には同じです。
スタートとゴールの位置から移動量を計算し、それにoffsetの値を掛けるだけです。
その際、場所は毎回リセットされてはこまるので、現在のページも計算に組み込む必要があります。

//list[background-icon][背景のアイコンの移動の計算]{
val onPageScrolled: (Int, Float, Int) -> Unit = { position, positionOffset, _ ->
  // positionは現在されているページ
  translationX = -(displayWidth * (position + positionOffset) * parallaxMultiplier)
}
//}

次は背景色の変化についてです。
背景色に関しては@<code>{ParallaxViewPagerBackgroundView}クラスで描画しています。
1ページ目から2ページ目へ移動する途中で背景は2種類登場します。
1つ目は1ページ目の背景色描画のための領域（@<code>{rectNow}）、
2つ目は2ページ目の背景色描画のための領域です（@<code>{rectNext}）。
これら2つの描画領域を求めて、@<code>{onDraw}で描画します。

今回は@<code>{rectNow}を描画した後に、@<code>{rectNext}を描画してみます。
@<code>{rectNow}は現在選択されているページの背景色をベタ塗りすれば簡単です。
@<code>{rectNext}だけを計算すればよいのです。

//list[background-color][背景色の描画]{
private val rectNow = RectF(0f, 0f, displayWidth, displayHeight)
private val rectNext = RectF()

val onPageScrolled: (Int, Float, Int) -> Unit = {
  position, positionOffset, positionOffsetPixels ->

    if (positionOffsetPixels == 0 || currentPosition.ordinal != position) {
        scrollState = if (positionOffset < 0.5f) ScrollState.FORWARD
          else ScrollState.BACKWARD
    }

    // 登録されているEnumによって、描画する背景を変えている
    if (currentPosition.ordinal != position) {
        currentPosition = WalkThroughPage.values()[position]
        when (currentPosition) {
            WalkThroughPage.DEVICE_1 -> {
                backgroundPaintNow.color = background1
                backgroundPaintNext.color = background2
            }
            WalkThroughPage.DEVICE_2 -> {
                backgroundPaintNow.color = background2
                backgroundPaintNext.color = background3
            }
            WalkThroughPage.DEVICE_3 -> {
                backgroundPaintNow.color = background3
                backgroundPaintNext.color = background1
            }
        }
    }

    rectNext.set(calcLeftX(positionOffset), 0f, displayWidth, displayHeight)

    invalidate()
}

// 描画すべき領域の計算
private fun calcLeftX(positionOffset: Float): Float {
    return if (scrollState == ScrollState.FORWARD) {
        displayWidth - (displayWidth * (positionOffset / parallaxDivisor))
    } else {
        displayWidth * ((1 - positionOffset) / parallaxDivisor)
    }
}

override fun onDraw(canvas: Canvas) {
    super.onDraw(canvas)
    canvas.drawRect(rectNow, backgroundPaintNow)
    canvas.drawRect(rectNext, backgroundPaintNext)
}
//}


@<code>{ViewPager.OnPageChangeListener#onPageScrolled}だけですべてのアニメーションが完結します。
タッチイベントから移動量を計算するよりはだいぶシンプルに実装できたと思います。

== 終わりに
本章では、FOLIOのAndroidアプリのウォークスルーの実装について説明しました。
説明を簡単にするために省きましたが、実際には画面回転や横画面のときのレイアウトなど、さらに細かい調整をしています。
（※説明のために実装を簡略化したり削った部分がかなりあります）

たかがウォークスルー、されどウォークスルー。
アプリで心地よいインタラクションを実装したいと思っている誰かの手助けになれば幸いです。
