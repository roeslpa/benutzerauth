import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

import org.jblas.ComplexDoubleMatrix;
import org.jblas.DoubleMatrix;
import org.jblas.Eigen;
import org.jblas.ranges.RangeUtils;

/**
 * THIS IS A TESTBENCH FOR PCA FACE RECOGNITION
 * @author Fabian Kammel <fabian.kammel@rub.de>
 * @author Jan Rimkus <jan.rimkus@rub.de>
 * 
 * PLEASE SUPPLY YOUR IMPLEMENTATION IN THE doPCA() FUNCTION
 * AND RETURN THE FOUND IMAGES AND PROBABILITIES AS EXPLAINED
 * IN THE FUNCTION DESCRIPTION
 * 
 * THE WHOLE PROJECT MUST COMPILE WITHOUT WARNINGS AND ERRORS 
 * 
 * ANY FURTHER INFORMATION CAN BE FOUND IN THE PROBLEM DESCRIPTION 
 * IF YOU NEED ANY ADDITIONAL HELP REGARDING THE PROGRAMMING
 * ASSIGMNMENT WRITE A MAIL TO:  
 * Maximilian Golla <maximilian.golla@rub.de>
 *
 */

public class PCA {
	
	private List<File> fileSet;
	private List<PGM> set;
	//private List<DoubleMatrix> set;
	private List<File> resultFile;
	private List<Double> resultDistance;
	
	private DoubleMatrix averageWeight;
	private DoubleMatrix X;
	private DoubleMatrix XT;
	private DoubleMatrix C;
	private DoubleMatrix F;
	private DoubleMatrix E;
	private DoubleMatrix Et;
	private DoubleMatrix EtT;
	private DoubleMatrix ET;
	private List<DoubleMatrix> Y;
	
	public PCA(List<File> fileSet) {
		set = new ArrayList<PGM>();
		try {
			for(int i=0; i<fileSet.size(); i++) {
				set.add(new PGM(new FileInputStream(fileSet.get(i)) , fileSet.get(i).getPath()));
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		
		this.resultFile = new ArrayList<File>(3);
		this.resultDistance = new ArrayList<Double>(3);
		
		this.averageWeight = new DoubleMatrix();		
		this.Y = new ArrayList<DoubleMatrix>();
		
		//Calculate average
		averageWeight = set.get(0).getMatrix();
		for(int i=1; i<set.size(); i++) {
			averageWeight = averageWeight.add(set.get(i).getMatrix());
		}
		averageWeight = averageWeight.muli(1.0/set.size());
		
		//Subtract average from all pictures
		X = set.get(0).getMatrix().sub(averageWeight);
		for(int i=0; i<set.size(); i++) {
			X = DoubleMatrix.concatHorizontally(X, set.get(i).getMatrix().sub(averageWeight));
		}
		XT = X.transpose();
		
		C = XT.mmul(X);
		F = Eigen.eigenvectors(C)[0].getReal();
		E = X.mmul(F);
		Et = E.getColumns(RangeUtils.interval(6, 305));
		EtT = E.transpose();
		//ET = E.transpose();
		
		for(int i=0; i<set.size(); i++) {
			Y.add(EtT.mmul(set.get(i).getMatrix()));
		}
		
		/* Debug
		averageWeight.print();
		X.print();
		XT.print();
		C.print();
		E.print();
		ET.print();
		Y.get(0).print();
		Y.get(1).print();
		*/
	}
	
	public List<File> getResultFiles() {
		return resultFile;
	}
	
	public List<Double> getResultDist() {
		return resultDistance;
	}
	
	private void addFileResult(int index, File f) {
		resultFile.add(index, f);
	}
	
	private void addDistResult(int index, Double d) {
		resultDistance.add(index, d);
	}
	
	/**
	 * Calculate the three most similar pictures from the 'set' of files, compared
	 * to the toTest File, supplied to the function. 
	 * 
	 * Write your results to the resultFile and resultDistance lists using the
	 * function addFileResult() and addDistResult(). Where index 0 is the
	 * file/image with the most similarities to the probe image, and index 2 is 
	 * the file/image with the least similarities to the probe image.
	 * 
	 * resultFile has to contain a File object
	 * resultDist has to contain the euclidean distance to the probe image
	 * 
	 * You can also chose to write directly to the result lists. 
	 * 
	 * Indexes >= 3 are not used in the resultFile and resultDistance lists. 
	 * 
	 * @param toTest The image that has to be compared to the set of files
	 * 
	 */
	public void doPCA(File toTestFile) {
	//public void doPCA(DoubleMatrix toTest) {
		PGM toTest;
		DoubleMatrix yAst;
		Double yDistI, yDist0, yDist1, yDist2;
		int first, second, third;
		
		toTest = null;
		
		try {
			toTest = new PGM(new FileInputStream(toTestFile) , toTestFile.getPath());
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		
		yDist0 = Double.MAX_VALUE;
		yDist1 = Double.MAX_VALUE;
		yDist2 = Double.MAX_VALUE;
		first = Integer.MAX_VALUE;
		second = Integer.MAX_VALUE;
		third = Integer.MAX_VALUE;
		
		yAst = EtT.mmul(toTest.getMatrix());
		
		for(int i = 0; i<Y.size(); i++) {
			yDistI = yAst.distance2(Y.get(i));
			
			if(yDistI.compareTo(yDist0) < 0) {
				yDist2 = yDist1;
				yDist1 = yDist0;
				yDist0 = yDistI;
				third = second;
				second = first;
				first = i;
			} else if(yDistI.compareTo(yDist1) < 0) {
				yDist2 = yDist1;
				yDist1 = yDistI;
				third = second;
				second = i;
			} else if(yDistI.compareTo(yDist2) < 0) {
				yDist2 = yDistI;
				third = i;
			}
		}
		
		//This is how you add files to the result
		addFileResult(0, new File(set.get(first).getPath()));
		addFileResult(1, new File(set.get(second).getPath()));
		addFileResult(2, new File(set.get(third).getPath()));
		
		//This is how you add the corresponding probs to the result
		addDistResult(0, yDist0);
		addDistResult(1, yDist1);
		addDistResult(2, yDist2);
		
		return;
	}

}
