import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.jblas.ComplexDoubleMatrix;
import org.jblas.DoubleMatrix;
import org.jblas.Eigen;

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
	
	private List<File> set;
	private List<File> resultFile;
	private List<Double> resultDistance;
	
	public PCA(List<File> set) {
		this.set = set;
		
		this.resultFile = new ArrayList<File>(3);
		this.resultDistance = new ArrayList<Double>(3);
		
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
	public void doPCA(File toTest) {
		
		
		/* DELETE THIS AND FILL IN YOUR OWN CODE */
		/* ------------------------------------- */
		
		//This is how you add files to the result
		addFileResult(0, toTest);
		addFileResult(1, toTest);
		addFileResult(2, toTest);
		
		//This is how you add the corresponding probs to the result
		addDistResult(0, 100.0);
		addDistResult(1, 90.0);
		addDistResult(2, 80.0);
		
		/* ------------------------------------- */
		/* DELETE THIS AND FILL IN YOUR OWN CODE */
		
		return;
	}

}
