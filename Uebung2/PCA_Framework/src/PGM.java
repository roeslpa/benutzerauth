import java.io.IOException;
import java.io.InputStream;

import javax.xml.bind.annotation.adapters.HexBinaryAdapter;

import org.jblas.DoubleMatrix;

/**
 * Skeleton class to encapsulate the PGM images. 
 * This class should save meta data about the image, such as width and height, 
 * and also create an array of pixels, which is used to create the DoubleMatrix
 * used to do the PCA calculation. 
 * The path attribute is also good to save, so you have a chance to map the .pgm
 * image back to the original File object. 
 * 
 * This is only a skeleton class. You can choose to use a constructor with a 
 * different signature as well, or not use this class at all. It is only here
 * to give guidance. 
 * 
 * @author Fabian Kammel <fabian.kammel@rub.de>
 * @author Jan Rimkus <jan.rimkus@rub.de>
 */

public class PGM {
	
	public int width;
	public int height;
	private double[] pixels;
	private DoubleMatrix y;
	private String path;
	
	
	public PGM(InputStream in, String path) {
		byte[] stream;

		/* Check Magic Bytes and read width and height, and calculate
		 * Array Size */
		try {
			//Read the standard values
			PGM.readNext(in);
			this.width = Integer.parseInt(PGM.readNext(in));
			this.height = Integer.parseInt(PGM.readNext(in));
			this.pixels = new double[width * height];
			stream = new byte[width * height];
			PGM.readNext(in);
			
			//Read the pixel values; & 0xFF because else 8 Bit integers become negative (129 => -127) but we need positive numbers
			in.read(stream);
			for(int i=0; i<stream.length; i++) {
				pixels[i] = (0xFF & stream[i]);
			}
			
			/* Create DoubleMatrix by reading from the File/InputStream */
			y = new DoubleMatrix(pixels);
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		/* Save path for later */
		this.path = path;
	}
	
	/* Some other helper methods you can choose to use, or delete */
	
	public double getElement(int pos) {
		if((pos < pixels.length) && (pos >= 0)) {
			return this.pixels[pos];
		}
		return Double.NaN;
	}
	
	public double[] getPixels() { 
		return (double[]) this.pixels.clone();
	}
	
	public DoubleMatrix getMatrix() {
		return new DoubleMatrix(this.pixels.clone());
	}
	
	public String getPath() {
		return path;
	}
	
	private static boolean isWhiteSpace(int c) {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}
	
	private static String readNext(InputStream in) throws IOException {
		byte[] b = new byte[1];
		String text = new String("");
		in.read(b);
		
		//Get the next string until whitespace (for first values)
		while(!isWhiteSpace(b[0])) {
			text = text.concat(new String(b));
			in.read(b);
		}
		return text;
	}
}
