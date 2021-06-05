/* ********
   テクスチャ合成　ヘッダファイル　　（作成者：藤本忠博）
   ******** */

#include <vector>
#include <opencv2/opencv.hpp>

#define OPENCV_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))

#ifndef _TS_TEXSYN_H
#define _TS_TEXSYN_H

   /* ========
	  マクロ */

// 入力テクスチャの最大解像度
#define TS_IN_XMAX 100
#define TS_IN_YMAX 100

// 出力テクスチャの最大解像度
#define TS_OUT_XMAX 400
#define TS_OUT_YMAX 400

// 多重解像度の最大レベル
#define MUL_LEVEL_MAX 10

/* ========
   関数宣言 */

   /* --------
	  全探索アルゴリズム
	*/
std::vector<cv::Mat> ms_full_search(
	int nbr,                // 近隣画素群（neighbor）の大きさ
							//   nbr >= 1
							// 近隣画素群の一辺が 2 * nbr + 1 となる
	int in_cyc,             // 入力テクスチャが環状かどうか
							//   0 : 環状でない
							//   1 : 環状である
	unsigned int out_seed,  // 出力テクスチャの初期化のための乱数の種
	float Reduction_rate,	// 縮小率
	std::vector<std::vector<std::vector<std::vector<uchar>>>> intex,     // 入力テクスチャ
	std::vector<std::vector<std::vector<std::vector<uchar>>>> outtex   // 出力テクスチャ
);

#endif

// ==== EOF
