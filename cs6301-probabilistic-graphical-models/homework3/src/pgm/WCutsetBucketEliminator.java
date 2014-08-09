package pgm;

import java.util.ArrayList;

import lib.LogNumber;

public class WCutsetBucketEliminator extends BucketEliminator {
  protected VariableCollectionType cutset = new VariableList();
  protected EvidenceCollectionType cutsetEvidence = new EvidenceHashMap();
  
  protected int maxTreeWidth;
  protected int maxSampleIterations;
  
  protected WCutsetBucketEliminatorSampleMode sampleMode;  
  
  WCutsetBucketEliminator(
      VariableCollectionType v, 
      EvidenceCollectionType e,
      ArrayList<FunctionTable> f,
      int maxTreeWidth,
      int maxSampleIterations,
      WCutsetBucketEliminatorSampleMode sampleMode) {
    super(v, e, f);
    this.maxTreeWidth = maxTreeWidth;
    this.maxSampleIterations = maxSampleIterations;
    this.sampleMode = sampleMode;
  }
  
  @Override
  public LogNumber eliminate() {
    
    
    ArrayList<VariableType> X = computeWCutset();
    
    
    
    return new LogNumber(0.0);
  }

  protected ArrayList<VariableType> computeWCutset() {
    // TODO Auto-generated method stub
    return null;
  }
  
  
}
