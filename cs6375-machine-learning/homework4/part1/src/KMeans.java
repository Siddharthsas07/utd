/*** Author :Vibhav Gogate
The University of Texas at Dallas
*****/

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.Random;

import javax.imageio.ImageIO;

public class KMeans {
  public static void main( String [] args ) {
  if ( args.length < 3 ) {
    System.out.println( "Usage: Kmeans <input-image> <k> <output-image>" );
    return;
  }

  try {
    // Read original image from file, run through kmeans, and output results.
    BufferedImage originalImage = ImageIO.read( new File(args[0]) );
    BufferedImage kmeansJpg = kmeans_helper( originalImage, Integer.parseInt(args[1]) );
    ImageIO.write( kmeansJpg, "png", new File(args[2]) );
  }

  catch ( IOException e ) {
    System.out.println( e.getMessage() );
  }
  }

  /**
   * @param originalImage - An image to compress
   * @param k - The number of clusters to use to compress originalImage
   * @return A compressed image using the kmeans algorithm
   */
  private static BufferedImage kmeans_helper( BufferedImage originalImage, int k ) {
  int w = originalImage.getWidth();
  int h = originalImage.getHeight();
  BufferedImage kmeansImage = new BufferedImage( w, h, originalImage.getType() );
  Graphics2D g = kmeansImage.createGraphics();
  g.drawImage( originalImage, 0, 0, w, h, null );

  // Read rgb values from the image.
  int[] imageRGB = new int[(w*h)];
  int counter = 0;
  for ( int i = 0; i < w; i++ ) {
    for( int j = 0; j < h; j++ ) {
         imageRGB[counter++] = kmeansImage.getRGB(i,j);
    }
  }

  // Call kmeans algorithm: update the rgb values to compress image.
  kmeans( imageRGB,k );

  // Write the new rgb values to the image.
  counter = 0;
  for( int i = 0; i < w; i++ ) {
    for( int j = 0; j < h; j++ ) {
        kmeansImage.setRGB( i, j, imageRGB[counter++] );
    }
  }

  // Return the compressed image
  return kmeansImage;
  }

  /**
   * Performs the k-means algorithm with specified k on the pixels
   * @param rgb - RGB values tightly packed for an image
   * @param k - The number of clusters to use for compression
   * @pre Assumes that you have at least k pixels.
   */
  private static void kmeans( int[] pixels, int k ) {
    if ( pixels.length < k ) {
      System.out.println( "You must supply at least k pixels." );
      return;
    }

    int[] previousCenters = new int[k];   // Previous iteration's cluster centers
    int[] currentCenters = new int[k];    // Current iteration's cluster centers
    int[] pixelsInCluster = new int[k];   // Number of pixels belonging to a cluster
    int[] clusterTotalAlpha = new int[k]; // Total alpha in a cluster
    int[] clusterTotalRed = new int[k];   // Total red in a cluster
    int[] clusterTotalGreen = new int[k]; // Total green in a cluster
    int[] clusterTotalBlue = new int[k];  // Total blue in a cluster
    int[] clusterAssignment = new int[pixels.length]; // Cluster assignment indices for each pixel
    int iterationCounter = 0;             // Number of iterations processed for k-means

    double maxDist = Double.MAX_VALUE;    // Used in k-means
    double curDist = 0;                   // Used in k-means
    int closestCenter = 0;                // Used in k-means

    // Seed initial cluster centers by picking k random pixels from the supplied
    // set. Do not allow repeats.
    for ( int i = 0; i < currentCenters.length; i++ ) {
      Random rng = new Random();
      int centerValue = 0;
      do {
        centerValue = pixels[rng.nextInt( pixels.length )];
      } while ( exists( centerValue, previousCenters ) );
      currentCenters[i] = centerValue;
    }

    // Perform k-means!
    do {
      //iterationCounter++;
      //System.out.println( iterationCounter );

      // Copy current centers into previous centers for convergence comparison later.
      // Also reset counts for the current iteration.
      for ( int i = 0; i < currentCenters.length; i++ ) {
        previousCenters[i] = currentCenters[i];
        pixelsInCluster[i] = 0;
        clusterTotalAlpha[i] = 0;
        clusterTotalRed[i] = 0;
        clusterTotalGreen[i] = 0;
        clusterTotalBlue[i] = 0;
      }

      // For each pixel, find its closest cluster center, update the
      // clusterAssignment for each pixel. Update cluster color counts.
      for ( int i = 0; i < pixels.length; i++ ) {
        maxDist = Double.MAX_VALUE;

        for ( int j = 0; j < currentCenters.length; j++ ) {
          curDist = pixelDist( pixels[i], currentCenters[j] );
          if ( curDist < maxDist ) {
            maxDist = curDist;
            closestCenter = j;
          }
        }

        clusterAssignment[i] = closestCenter;
        pixelsInCluster[closestCenter]++;
        clusterTotalAlpha[closestCenter] += ((pixels[i] & 0xFF000000) >>> 24);
        clusterTotalRed[closestCenter] +=   ((pixels[i] & 0x00FF0000) >>> 16);
        clusterTotalGreen[closestCenter] += ((pixels[i] & 0x0000FF00) >>> 8);
        clusterTotalBlue[closestCenter] +=  ((pixels[i] & 0x000000FF) >>> 0);
      }

      // Update cluster centers to the average of all pixels belonging to cluster.
      // Average A, R, G, and B components individually and then pack into center.
      for ( int i = 0; i < currentCenters.length; i++ ) {
        int averageAlpha = (int)((double)clusterTotalAlpha[i] / (double)pixelsInCluster[i]);
        int averageRed =   (int)((double)clusterTotalRed[i] /   (double)pixelsInCluster[i]);
        int averageGreen = (int)((double)clusterTotalGreen[i] / (double)pixelsInCluster[i]);
        int averageBlue =  (int)((double)clusterTotalBlue[i] /  (double)pixelsInCluster[i]);

        currentCenters[i] = ((averageAlpha & 0x000000FF) << 24) |
                            ((averageRed & 0x000000FF) << 16) |
                            ((averageGreen & 0x000000FF) << 8) |
                            ((averageBlue & 0x000000FF) << 0);
      }
    } while( !converged(previousCenters, currentCenters) );

    // k-means has converged. Assign each pixel the value of its center.
    for ( int i = 0; i < pixels.length; i++ ) {
      pixels[i] = currentCenters[clusterAssignment[i]];
    }
  }

  /**
   * Checks to see if value exists in the array
   * @param value - The value to search the array for
   * @param array - The array to search for value in
   * @return true if value exists in the array, false otherwise
   */
  private static boolean exists( int value, int[] array ) {
    for ( int i = 0; i < array.length; i++ )
      if ( array[i] == value )
        return true;

    return false;
  }

  /**
   * Checks to see if the values in the two arrays have converged
   * @param a - The first array
   * @param b - The second array
   * @return true if the arrays have converged
   */
  private static boolean converged( int[] a, int[] b ) {
    for ( int i = 0; i < a.length; i++ )
      if ( a[i] != b[i] )
        return false;

    return true;
  }

  /**
   * Treats pixA and pixB as 4D points whose values are tightly packed and
   * calculates the Euclidian distance between them. Pixels are stored according to
   * aorder A, B, G, R from lower to higher byte addresses within each pixel.
   * @param pixA - The first pixel
   * @param pixB - The second pixel
   * @return The Euclidian distance between pixA and pixB
   */
  private static double pixelDist( int pixA, int pixB ) {
    int aDiff = ((pixA & 0xFF000000) >>> 24) - ((pixB & 0xFF000000) >>> 24);
    int rDiff = ((pixA & 0x00FF0000) >>> 16) - ((pixB & 0x00FF0000) >>> 16);
    int gDiff = ((pixA & 0x0000FF00) >>> 8)  - ((pixB & 0x0000FF00) >>> 8);
    int bDiff = ((pixA & 0x000000FF) >>> 0)  - ((pixB & 0x000000FF) >>> 0);
    return Math.sqrt( aDiff*aDiff + rDiff*rDiff + gDiff*gDiff + bDiff*bDiff );
  }
}
