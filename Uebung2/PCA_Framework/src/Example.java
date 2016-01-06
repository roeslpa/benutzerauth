import java.util.ArrayList;
import java.util.List;

import org.jblas.DoubleMatrix;


public class Example {
	
	private static final int N = 2; //TODO: Put number of images in here
	private static final int doubleOcc = 1; //TODO: Put number of images per person here
	private static final int K = 2; //TODO: Put number of images in subset here
	private static final List<Integer> S = new ArrayList<Integer>(N);
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ArrayList<DoubleMatrix> set = new ArrayList<DoubleMatrix>();
		
		double[][] pic1 = {{2.0}, {2.0}, {1.0}};
		double[][] pic2 = {{0.0}, {0.0}, {1.0}};
		double[][] test = {{2.0}, {1.0}, {1.0}};
		
		set.add(new DoubleMatrix(pic1));
		set.add(new DoubleMatrix(pic2));
		
		PCA pca = new PCA(set);
		
		pca.doPCA(new DoubleMatrix(test));
	}

}
