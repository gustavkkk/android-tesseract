package com.dynamsoft.tessocr;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.R.string;
import android.animation.IntArrayEvaluator;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class OCRActivity extends Activity implements OnClickListener {
	private TessOCR mTessOCR;
	private TextView mResult;
	private ProgressDialog mProgressDialog;
	private ImageView mImage;
	private Button mButtonGallery, mButtonCamera;
	private String mCurrentPhotoPath;
	private static final int REQUEST_TAKE_PHOTO = 1;
	private static final int REQUEST_PICK_PHOTO = 2;
	//
	public static final String DATA_PATH = Environment
			.getExternalStorageDirectory().toString() + "/tesseract/";
	public static final String lang = "chi_sim";
	private static final String TAG = "OCRActivity.java";
	//
    static {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("locateString");
    }
    public static native void locatestring(int width, int height, byte[] original, int rectsize, int[] rectlist);
    public static native void concatstring(int width, int height, byte[] singleLine);
    public static native void getsinglestringrect(int width, int height, int margin, int index, byte[] singleLine);
	//
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		String[] paths = new String[] { DATA_PATH, DATA_PATH + "tessdata/" };

		for (String path : paths) {
			File dir = new File(path);
			if (!dir.exists()) {
				if (!dir.mkdirs()) {
					Log.v(TAG, "ERROR: Creation of directory " + path + " on sdcard failed");
					Log.d("load_file_test", "ERROR: Creation of directory " + path + " on sdcard failed");
					return;
				} else {
					Log.v(TAG, "Created directory " + path + " on sdcard");
					Log.d("load_file_test", "Created directory " + path + " on sdcard");
				}
			}

		}	
		if (!(new File(DATA_PATH + "tessdata/" + lang + ".traineddata")).exists()) {
			try {

				AssetManager assetManager = getAssets();
				InputStream in = assetManager.open("tessdata/chi_sim.traineddata");

				OutputStream out = new FileOutputStream(DATA_PATH + "tessdata/chi_sim.traineddata");

				byte[] buf = new byte[1024];
				int len;
				
				while ((len = in.read(buf)) > 0) {
					out.write(buf, 0, len);
				}
				in.close();
				out.close();				
				Log.v(TAG, "Copied " + lang + " traineddata");
			} catch (IOException e) {
				Log.e(TAG, "Was unable to copy " + lang + " traineddata " + e.toString());
			}
		}	
		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		mResult = (TextView) findViewById(R.id.tv_result);
		mImage = (ImageView) findViewById(R.id.image);
		mButtonGallery = (Button) findViewById(R.id.bt_gallery);
		mButtonGallery.setOnClickListener(this);
		mButtonCamera = (Button) findViewById(R.id.bt_camera);
		mButtonCamera.setOnClickListener(this);
		mTessOCR = new TessOCR();
	}

	private void uriOCR(Uri uri) {
		if (uri != null) {
			InputStream is = null;
			try {
				is = getContentResolver().openInputStream(uri);
				Bitmap bitmap = BitmapFactory.decodeStream(is);
				mImage.setImageBitmap(bitmap);//kojy-todo
				doOCR(bitmap);
				//mImage.setImageBitmap(test_jni_locaterect);//kojy-todo
				//mImage.refreshDrawableState();
				
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} finally {
				if (is != null) {
					try {
						is.close();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}
	
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();

		Intent intent = getIntent();
		if (Intent.ACTION_SEND.equals(intent.getAction())) {
			Uri uri = (Uri) intent
					.getParcelableExtra(Intent.EXTRA_STREAM);
			uriOCR(uri);
		}
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();

		mTessOCR.onDestroy();
	}

	private void dispatchTakePictureIntent() {
		Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		// Ensure that there's a camera activity to handle the intent
		if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
			// Create the File where the photo should go
			File photoFile = null;
			try {
				photoFile = createImageFile();
			} catch (IOException ex) {
				// Error occurred while creating the File

			}
			// Continue only if the File was successfully created
			if (photoFile != null) {
				takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT,
						Uri.fromFile(photoFile));
				startActivityForResult(takePictureIntent, REQUEST_TAKE_PHOTO);
			}
		}
	}

	/**
	 * http://developer.android.com/training/camera/photobasics.html
	 */
	private File createImageFile() throws IOException {
		// Create an image file name
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss")
				.format(new Date());
		String imageFileName = "JPEG_" + timeStamp + "_";
		String storageDir = Environment.getExternalStorageDirectory()
				+ "/TessOCR";
		File dir = new File(storageDir);
		if (!dir.exists())
			dir.mkdir();

		File image = new File(storageDir + "/" + imageFileName + ".jpg");

		// Save a file: path for use with ACTION_VIEW intents
		mCurrentPhotoPath = image.getAbsolutePath();
		return image;
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		if (requestCode == REQUEST_TAKE_PHOTO
				&& resultCode == Activity.RESULT_OK) {
			setPic();
		}
		else if (requestCode == REQUEST_PICK_PHOTO
				&& resultCode == Activity.RESULT_OK) {
			Uri uri = data.getData();
			if (uri != null) {
				uriOCR(uri);
			}
		}
	}

	private Bitmap test_jni_locaterect;
	private void setPic() {
		// Get the dimensions of the View
		int targetW = mImage.getWidth();
		int targetH = mImage.getHeight();

		// Get the dimensions of the bitmap
		BitmapFactory.Options bmOptions = new BitmapFactory.Options();
		bmOptions.inJustDecodeBounds = true;
		BitmapFactory.decodeFile(mCurrentPhotoPath, bmOptions);
		int photoW = bmOptions.outWidth;
		int photoH = bmOptions.outHeight;

		// Determine how much to scale down the image
		int scaleFactor = Math.min(photoW / targetW, photoH / targetH);

		// Decode the image file into a Bitmap sized to fill the View
		bmOptions.inJustDecodeBounds = false;
		bmOptions.inSampleSize = scaleFactor << 1;
		bmOptions.inPurgeable = true;

		Bitmap bitmap = BitmapFactory.decodeFile(mCurrentPhotoPath, bmOptions);
		mImage.setImageBitmap(bitmap);
		doOCR(bitmap);
		//mImage.setImageBitmap(test_jni_locaterect);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		switch (id) {
		case R.id.bt_gallery:
			pickPhoto();
			break;
		case R.id.bt_camera:
			takePhoto();
			break;
		}
	}
	
	private void pickPhoto() {
		Intent intent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
		startActivityForResult(intent, REQUEST_PICK_PHOTO);
	}

	private void takePhoto() {
		dispatchTakePictureIntent();
	}

	public static Bitmap readBitmapFromByteArray(byte[] data, int width, int height) {
        BitmapFactory.Options options = new BitmapFactory.Options();
//        options.inJustDecodeBounds = true;
//        BitmapFactory.decodeByteArray(data, 0, data.length, options);
//        float srcWidth = options.outWidth;
//        float srcHeight = options.outHeight;
//        int inSampleSize = 1;
//
//        if (srcHeight > height || srcWidth > width) {
//            if (srcWidth > srcHeight) {
//                inSampleSize = Math.round(srcHeight / height);
//            } else {
//                inSampleSize = Math.round(srcWidth / width);
//            }
//        }
//
//        options.inJustDecodeBounds = false;
//        options.inSampleSize = inSampleSize;
        options.inMutable = true;
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;

        return BitmapFactory.decodeByteArray(data, 0, data.length, options);
    }
	
	public Bitmap byte2bmp(byte[] data, int width, int height)
	{
		Bitmap.Config config;
	    Bitmap bm = Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
	    for(int j=0; j<height; j++){
	       for(int i=0; i<width; i++){
	    	   int gray = data[width*j + i];
	    	   int alpha = 0xffffffff;
	    	   bm.setPixel(i, j, Color.argb(alpha, gray, gray, gray));
	       }
	    }
	    return bm;
	}
	private String correctString(String tmp)
	{
		String str = tmp;
		if(tmp.contains("筐"))
			str = tmp.replace("筐", "管");
		if(tmp.contains("顺牙"))
			str = tmp.replace("顺牙", "蓝牙");
		if(tmp.contains("禾口"))
			str = tmp.replace("禾口", "和");
		if(tmp.contains("其月"))
			str = tmp.replace("其月", "期");
		if(tmp.contains("黑犬认"))
			str = tmp.replace("黑犬认", "默认");	
		if(tmp.contains("说阴"))
			str = tmp.replace("说阴", "说明");	
		if(tmp.contains("蚤份"))
			str = tmp.replace("蚤份", "备份");
		if(tmp.contains("矢口"))
			str = tmp.replace("矢口", "知");
		if(tmp.contains("耳又"))
			str = tmp.replace("耳又", "取");
		return str;
	}
	private void doOCR(final Bitmap bitmap) {
		if (mProgressDialog == null) {
			mProgressDialog = ProgressDialog.show(this, "Processing",
					"Doing OCR...", true);
		}
		else {
			mProgressDialog.show();
		}
		
		new Thread(new Runnable() {
			public void run() {
				//
				long start = System.currentTimeMillis();
				//

				boolean useDefaultMode = true,
						singleLineMode = false;
				String conca_all = new String();
				if(useDefaultMode)
				{
					//Bitmap to JNI
					int b = 1;
					int width = bitmap.getWidth();
					int height = bitmap.getHeight();
					switch (bitmap.getConfig()) {
					    case ALPHA_8:
					        b = 1;
					        break;
					    case ARGB_4444:
					        b = 2;
					        break;
					    case ARGB_8888:
					        b = 4;
					        break;
					}
					//
			        ByteBuffer buf = ByteBuffer.allocate(width * height * b);
			        bitmap.copyPixelsToBuffer(buf);
			        
			        int stringcount_max = 40;
			        int rectsize = stringcount_max * 4;
			        IntBuffer rects = IntBuffer.allocate(rectsize);
			        int []rectlist = new int[rectsize + 1];
			        locatestring(width,height,buf.array(),rectsize,rectlist);
					//Mat to Bitmap   

					Log.d("OCR_test_java", rectlist[0]+"");
					if(!singleLineMode)
					{
				        for(int i = 1; i < rectlist[0]; i+=4)//rectlist[0] == the real number of rects
				        {
				        	String str;
				        	boolean isWhat = false;
				        	if(isWhat)//
				        	{
				        		Matrix m = new Matrix();
								m.postRotate(0);
								m.preScale(1, 1);
						        Bitmap roi = Bitmap.createBitmap(bitmap, rectlist[i], rectlist[i + 1], rectlist[i + 2], rectlist[i + 3], m, false);
						        str = mTessOCR.getOCRResult(roi);
				        	}
				        	else
				        	{
				        		int margin = 40;
				        		int h = rectlist[i + 3] + margin,w = rectlist[i + 2] + margin;
				        		byte[] imagedata = new byte[w * h];
				        		getsinglestringrect(w, h, margin/2, i / 4, imagedata);
				        		String tmpStr = mTessOCR.getOCRResult_gray(imagedata, w, h);
				        		//correct String Error
				        		str = correctString(tmpStr);
				        		
						        //Debug-Mode-LocateRect
						        boolean doDebug = true;
						        if(doDebug && i == (rectlist[0] - 3))
						        {
							        Bitmap tmp  = byte2bmp(imagedata, w, h);
									int targetW = mImage.getWidth();
									int targetH = mImage.getHeight();
									float scaleWidth = ((float) targetW) / w;  
								    float scaleHeight = ((float) targetH) / h;  
							        Matrix matrix = new Matrix();  
							        matrix.postScale(scaleWidth, scaleHeight); 
							        test_jni_locaterect = Bitmap.createBitmap(tmp, 0, 0, w, h, matrix, true);  
						        }
						        //
				        	}
					        conca_all += "coord=" +
					        			 "(" + rectlist[i] + "," +
					        				   rectlist[i + 1] + "," +
					        				   rectlist[i + 2] + "," +
					        				   rectlist[i + 3] +
					        			  ")" + " str = " +
					        			  str + "\n";
				        }
					}
					else
					{
						int height_single = 0,
							width_single = 0;
				        for(int i = 1; i < rectlist[0]; i+=4)//rectlist[0] == the real number of rects
				        {
				    		if (height_single < rectlist[i + 3])
				    			height_single = rectlist[i + 3];
				    		width_single += rectlist[i + 2];
				        }
				        byte []bitmapdata = new byte[height_single * width_single * 4];		        				        
				        concatstring(width_single, height_single, bitmapdata);			        			        
				        conca_all = mTessOCR.getOCRResult_rgba(bitmapdata, width_single, height_single);
					}
				}
				else
				{
					conca_all = mTessOCR.getOCRResult(bitmap);
				}
				final String result = conca_all;
				
				//
				long end = System.currentTimeMillis();
				//
				Log.d("OCR_test_java", "elapsed time = " + (end - start) +"ms");
				//
				runOnUiThread(new Runnable() {

					@Override
					public void run() {
						// TODO Auto-generated method stub
						if (result != null && !result.equals("")) {
							mResult.setText(result);
						}

						mProgressDialog.dismiss();
					}

				});

			};
		}).start();
	}
}
