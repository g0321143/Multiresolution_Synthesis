/* ********
   テクスチャ合成　全探索アルゴリズム　　（作成者：藤本忠博）
   ******** */

   /* ========
	  ヘッダファイル */

#include <stdio.h>
	  // #include <math.h>
#include <stdlib.h>
// #include <time.h>
// #include <opencv2/opencv.hpp>
#include "mutsyn.h"
#include <iostream>
/* ========
   関数 */

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
)
{

	/* ========
	   初期処理 */

	   // 各テクスチャの解像度
	std::vector<int> in_x(intex.size(), 0); // 入力テクスチャのx解像度
	std::vector<int> in_y(intex.size(), 0); // 入力テクスチャのy解像度
	std::vector<int> out_x(outtex.size(), 0); // 出力テクスチャのx解像度
	std::vector<int> out_y(outtex.size(), 0); // 出力テクスチャのy解像度

	for (int i = 0; i < intex.size(); i++) {
		in_x[i] = intex[i][0].size();
		in_y[i] = intex[i].size();
		out_x[i] = outtex[i][0].size();
		out_y[i] = outtex[i].size();
	}

	// 入力テクスチャの探索範囲
	std::vector<int> in_x_min(intex.size(), 0), in_x_max(intex.size(), 0);
	std::vector<int> in_y_min(intex.size(), 0), in_y_max(intex.size(), 0);
	for (int i = 0; i < intex.size(); i++) {
		if (in_cyc == 0) { // 環状でない
			in_x_min[i] = nbr;
			in_x_max[i] = in_x[i] - nbr;
			in_y_min[i] = nbr;
			in_y_max[i] = in_y[i];
		}
		else {              // 環状である
			in_x_min[i] = 0;
			in_x_max[i] = in_x[i];
			in_y_min[i] = 0;
			in_y_max[i] = in_y[i];
		}
	}

	// 出力テクスチャの初期化
	srand(out_seed);
	for (int l = 0; l < intex.size(); l++) {
		for (int oy = 0; oy < out_y[l]; oy++) {
			for (int ox = 0; ox < out_x[l]; ox++) {
				for (int c = 0; c < 3; c++) {
					outtex[l][oy][ox][c] = intex[l][rand() % in_y[l]][rand() % in_x[l]][c];
				}
			}
		}
	}

	/* ========
	   出力テクスチャの合成 */

	   // L型近隣画素群中の画素数
	int pxn = ((2 * nbr + 1) * (2 * nbr + 1)) / 2;

	/* ++++ print */
	std::cout << "ms_full_search() : "
		<< "nbr = " << nbr
		<< " , pxn = " << pxn
		<< " , level = " << intex.size()
		<< std::endl;

	// 最大 SSD (sum of squared differences) 値
	int ssd_max = 3 * 255 * 255 * pxn;


	// 出力テクスチャの画素のループ
	for (int l = intex.size() - 1; l >= 0; l--) {
		for (int oy = 0; oy < out_y[l]; oy++) {
			for (int ox = 0; ox < out_x[l]; ox++) {
				//std::cout << "l =" << l << " , oy =" << oy << " , ox =" << ox << ", out_y[l] = " << out_y[l] << " , out_x[l] = " << out_x[l] << std::endl;

				// 最小 SSD 値の初期化
				int ssd_min = ssd_max;

				// 入力テクスチャの選択画素の初期化
				int ix_s = -1;
				int iy_s = -1;
				int level = l;


				// 現在のレベルで探索しているピクセル座標に対応する１レベル前のピクセル座標
				int ox_r = ox * Reduction_rate;
				int oy_r = oy * Reduction_rate;
				//std::cout << "ox_r, oy_r = " << ox_r << " , " << oy_r << std::endl;

				// 正方領域のnbr
				int nbr_square = nbr;
				int pxn_square = ((2 * nbr_square + 1) * (2 * nbr_square + 1));


				// 入力テクスチャ上の探索
				for (int l_ = l + 1; l_ < intex.size(); l_++) {

					// 最大２レベル後ろのレベルまで含める
					if (l_ - l > 1) { break; }

					nbr_square = nbr_square - 1;
					pxn_square = ((2 * nbr_square + 1) * (2 * nbr_square + 1));
					//std::cout << "pxn_square = " << pxn_square << std::endl;

					//std::cout << "pxn = " << pxn << " , pxn_square = " << pxn_square << std::endl;

					for (int iy = in_y_min[l_]; iy < in_y_max[l_]; iy++) {
						for (int ix = in_x_min[l_]; ix < in_x_max[l_]; ix++) {

							// 近隣画素群の SSD 値の計算
							int s;
							int ssd = 0;

							for (int ny = (-nbr_square); ny <= nbr_square; ny++) {
								for (int nx = (-nbr_square); nx <= nbr_square; nx++) {

									// 計算
									for (int c = 0; c < 3; c++) {
										s = (int)outtex[l_][(oy_r + ny + out_y[l_]) % out_y[l_]][(ox_r + nx + out_x[l_]) % out_x[l_]][c]
											- (int)intex[l_][(iy + ny + in_y[l_]) % in_y[l_]][(ix + nx + in_x[l_]) % in_x[l_]][c];
										ssd += (s * s);
									}

								} // ny
							} // nx

							// 重み付け
							//ssd = ssd * (pxn_square / pxn);
							//std::cout << "pxn_square = " << pxn_square << " , pxn = " << pxn << std::endl;
							//std::cout << "ssd = " << ssd << " , float(pxn / pxn_square) = " << float(pxn) / float(pxn_square);
							//ssd = ssd * (float(pxn) / float(pxn_square));
							//std::cout << " , ssd after = " << ssd << std::endl;
							// 最小 SSD 値と入力テクスチャの選択画素の更新
							if (ssd_min > ssd) {
								ssd_min = ssd;
								ix_s = ix;
								iy_s = iy;
								level = l_;
							}
						}// iy
					}// ix

					ox_r *= Reduction_rate;
					oy_r *= Reduction_rate;
					//std::cout << "ox_r, oy_r = " << ox_r << " , " << oy_r << std::endl;
				}// l_


			// 入力テクスチャ上の探索
				for (int iy = in_y_min[l]; iy < in_y_max[l]; iy++) {
					for (int ix = in_x_min[l]; ix < in_x_max[l]; ix++) {

						// 近隣画素群の SSD 値の計算
						int s;
						int ssd = 0;

						// L型領域
						for (int ny = (-nbr); ny <= 0; ny++) {
							for (int nx = (-nbr); nx <= nbr; nx++) {
								// 近隣画素群の中心に来たら計算終了
								if ((ny == 0) && (nx == 0)) {
									break;
								}

								// 計算
								for (int c = 0; c < 3; c++) {
									s = (int)outtex[l][(oy + ny + out_y[l]) % out_y[l]][(ox + nx + out_x[l]) % out_x[l]][c]
										- (int)intex[l][(iy + ny + in_y[l]) % in_y[l]][(ix + nx + in_x[l]) % in_x[l]][c];
									ssd += (s * s);
								}
							} // ny
						} // nx


						// 最小 SSD 値と入力テクスチャの選択画素の更新
						if (ssd_min > ssd) {
							ssd_min = ssd;
							ix_s = ix;
							iy_s = iy;
							level = l;
						}

					} // iy
				} // ix

				// 出力テクスチャの画素色
				if (ssd_min == ssd_max) {  // 入力テクスチャの画素が選択されていない
					for (int c = 0; c < 3; c++) {
						outtex[l][oy][ox][c] = 0;
					}
				}
				else {                      // 入力テクスチャの画素が選択されている
					for (int c = 0; c < 3; c++) {
						//std::cout << "intex[" << level << "][" << iy_s << "][" << ix_s << "][c]" << std::endl;
						outtex[l][oy][ox][c] = intex[level][iy_s][ix_s][c];
					}
				} // end if

				//if (l != level) {
				//	std::cout << "正方領域" << std::endl;
				//}
				//else
				//{
				//	std::cout << "L型領域" << std::endl;
				//}

			} // ox
		} // oy
	} // level

	/* ========
	   終了処理 */

	   // 領域確保
	std::vector<cv::Mat> outtex_cv(intex.size());
	for (int i = 0; i < intex.size(); i++) {
		outtex_cv[i].create(outtex[i].size(), outtex[i][0].size(), CV_8UC3);
	}

	// 配列からのコピー
	for (int l = 0; l < intex.size(); l++) {
		for (int y = 0; y < outtex[l].size(); y++) {
			for (int x = 0; x < outtex[l][y].size(); x++) {
				for (int c = 0; c < 3; c++) {
					outtex_cv[l].at<cv::Vec3b>(y, x)[c] = outtex[l][y][x][c];
				}
			}
		}
	}
	// 正常終了
	return outtex_cv;
}

// ==== EOF
