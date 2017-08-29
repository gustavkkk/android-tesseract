package com.dynamsoft.tessocr;

import java.io.File;

import android.graphics.Bitmap;
import android.os.Environment;

import com.googlecode.tesseract.android.TessBaseAPI;

public class TessOCR {
	private TessBaseAPI mTess;
	
	public TessOCR() {
		// TODO Auto-generated constructor stub
		mTess = new TessBaseAPI();
		String datapath = Environment.getExternalStorageDirectory() + "/tesseract/";
		String language = "chi_sim";
		File dir = new File(datapath + "tessdata/");
		if (!dir.exists()) 
			dir.mkdirs();
		mTess.init(datapath, language);
	}
	
	public String getOCRResult(Bitmap bitmap) {
		
		mTess.setImage(bitmap);
		String result = mTess.getUTF8Text();

		return result;
    }
	
	public String getOCRResult_rgba(byte[] imagedata,int width,int height) {
		
		int bpp = 4,bpl = width * 4;
		mTess.setImage(imagedata, width, height, bpp, bpl);
		String result = mTess.getUTF8Text();

		return result;
    }
	
	public String getOCRResult_gray(byte[] imagedata,int width,int height) {
		
		int bpp = 1,bpl = width;
		mTess.setImage(imagedata, width, height, bpp, bpl);
		String result = mTess.getUTF8Text();

		return result;
    }
	
	public void onDestroy() {
		if (mTess != null)
			mTess.end();
	}
	
}
