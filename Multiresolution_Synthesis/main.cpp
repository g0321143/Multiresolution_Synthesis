/* ********
   テクスチャ合成　メイン関数　　（作成者：藤本忠博）

   参考ページ
   Texture Analysis and Synthesis
   http://graphics.stanford.edu/projects/texture/

   次のページからテクスチャ画像が入手できる．
   Texture Synthesis Examples
   http://graphics.stanford.edu/projects/texture/demo/

   ******** */

   /* ========
	  ヘッダファイル */

#include <stdio.h>
	  // #include <math.h>
	  // #include <stdlib.h>
#include <time.h>
#include "mutsyn.h"


/* ========
   マクロ */


   /* ========
	  グローバル変数 */

	  // **** テクスチャのフォルダ
char TS_folder[] = "./textures/";

// **** 近隣画素群（neighbor）の大きさ
//   nbr >= 1
// 近隣画素群の一辺が 2 * nbr + 1 となる
int TS_nbr = 7;

// **** 多重解像度化
//  多重解像度化レベル
int Mul_level = 4;
// 縮小率
float Reduction_rate = 0.5;


// **** 入力テクスチャ
// ファイル名
char TS_infile[] = "texture_o_icon.jpg";
// 環状かどうか
//   0 : 環状でない
//   1 : 環状である
int TS_in_cyc = 0;
// 解像度
int TS_in_x;
int TS_in_y;

// **** 出力テクスチャ
// 初期化のための乱数の種
unsigned int TS_out_seed = 1;
// 解像度
int TS_out_x = 128;
int TS_out_y = 128;

/* ========
   関数宣言 */


   /* ========
	  関数 */

	  /* --------
		 メイン関数
	   */
int main(void)
{
	// 入力テクスチャ
	// unsigned char intex[MUL_LEVEL_MAX][TS_IN_YMAX][TS_IN_XMAX][3]; オーバーフローした
	std::vector<std::vector<std::vector<std::vector<uchar>>>> intex(Mul_level);
	std::vector<cv::Mat> intex_cv(Mul_level);   // OpenCV
	// 出力テクスチャ
	// unsigned char outtex[MUL_LEVEL_MAX][TS_OUT_YMAX][TS_OUT_XMAX][3]; オーバーフローした
	std::vector<std::vector<std::vector<std::vector<uchar>>>> outtex(Mul_level);
	std::vector<cv::Mat> outtex_cv(Mul_level);  // OpenCV
	// フォルダ＋ファイル名
	char file[512];

	// ++++ 計算時間の計測
	clock_t time_start, time_end;
	double  time_sec;
	int     time_m;
	double  time_s;

	/* ========
	   初期処理 */

	std::cout << "OpenCV version: " << CV_VERSION << std::endl;

	/* ++++ 計算時間の計測開始 */
	printf("main() ----> start\n");
	time_start = clock();

	/* ========
	   入力テクスチャの入力 */

	// ファイルからの入力，表示
	strcpy_s(file, sizeof(file), TS_folder);
	strcat_s(file, sizeof(file), TS_infile);
	intex_cv[0] = cv::imread(file);
	TS_in_x = intex_cv[0].cols;
	TS_in_y = intex_cv[0].rows;
	cv::namedWindow("input_0", cv::WINDOW_AUTOSIZE);
	cv::imshow("input_0", intex_cv[0]);

	/* ++++ print */
	printf("main() : TS_infile >> %s\n", TS_infile);
	printf("intex_cv.cols, rows = %d, %d\n", intex_cv[0].cols, intex_cv[0].rows);

	// 配列の領域確保
	for (int i = 0; i < intex.size(); i++) {
		intex[i].resize(int(TS_in_y * std::pow(Reduction_rate, i)), 
			std::vector<std::vector<uchar>>(int(TS_in_x * std::pow(Reduction_rate, i)), std::vector<uchar>(3)));
		outtex[i].resize(int(TS_out_y * std::pow(Reduction_rate, i)),
			std::vector<std::vector<uchar>>(int(TS_out_x * std::pow(Reduction_rate, i)), std::vector<uchar>(3)));
	}
	std::cout << "intex_cv[" << 0 << "] = " << intex_cv[0].size() << std::endl;

	// テクスチャの低解像度化
	for (int i = 1; i < intex_cv.size(); i++) {
		intex_cv[i].create(intex[i].size(), intex[i][0].size(), CV_8UC3);
		cv::resize(intex_cv[i - 1], intex_cv[i], cv::Size(), Reduction_rate, Reduction_rate, cv::INTER_NEAREST);
		
		std::cout << "intex_cv[" << i << "] = " << intex_cv[i].size() << std::endl;
		char texUniName[64];
		sprintf_s(texUniName, "input_%d", i);
		cv::namedWindow(texUniName, cv::WINDOW_AUTOSIZE);
		cv::imshow(texUniName, intex_cv[i]);
	}


	// 配列へのコピー
	for (int l = 0; l < Mul_level; l++) {
		for (int y = 0; y < intex[l].size(); y++) {
			for (int x = 0; x < intex[l][y].size(); x++) {
				for (int c = 0; c < 3; c++) {
					intex[l][y][x][c] = intex_cv[l].at<cv::Vec3b>(y, x)[c];
					//printf("%d\n", intex[l][y][x][c]);
				}
			}
		}
	}

	/* ========
	   出力テクスチャの合成 */

	   /* 繰り返しコピー
	   for( int y = 0; y < TS_out_y; y++ ) {
		   for( int x = 0; x < TS_out_x; x++ ) {
			   for( int c = 0; c < 3; c++ ) {
				   outtex[y][x][c] = intex[y%TS_in_y][x%TS_in_x][c];
			   }
		   }d
	   } */
	
	 /* 全探索アルゴリズム */
	outtex_cv = ms_full_search(
		TS_nbr, TS_in_cyc, TS_out_seed, Reduction_rate,
		intex, outtex);

	/* ========
	   出力テクスチャの出力 */
	// ファイルへの出力，表示
	for (int i = 0; i < outtex_cv.size(); i++) {
		char texUniName[64];
		sprintf_s(texUniName, "output_%d", i);
		cv::namedWindow(texUniName, cv::WINDOW_AUTOSIZE);
		cv::imshow(texUniName, outtex_cv[i]);

		char TS_outfile[64];
		sprintf_s(TS_outfile, "output_%d.jpg", i);
		strcpy_s(file, sizeof(file), TS_folder);
		strcat_s(file, sizeof(file), TS_outfile);
		cv::imwrite(file, outtex_cv[i]);

		/* ++++ print */
		std::cout << "main() : TS_outfile >> " << TS_outfile << std::endl;
		std::cout << "outtex_cv[" << i << "].cols = " << outtex_cv[i].cols
			<< " , rows = " << outtex_cv[i].rows << std::endl;
	}

	/* ========
	   終了処理 */

	   /* ++++ 計算時間の計測終了 */
	time_end = clock();
	time_sec = (double)(time_end - time_start) / (double)CLOCKS_PER_SEC;
	time_m = (int)(time_sec / 60.0);
	time_s = time_sec - (double)time_m * 60.0;
	printf("main() : time >> %lf sec. = %d min. %lf sec.\n", time_sec, time_m, time_s);
	printf("main() <---- end\n");

	// キー入力待ち
	cv::waitKey(0);

	// 正常終了
	return(0);

}

// ==== EOF
