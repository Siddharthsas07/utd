package utd.cs.pgm.learner;

import utd.cs.pgm.core.graphmodel.GraphModel;
import utd.cs.pgm.util.LogDouble;

public interface IModelLearner {
  GraphModel getLearnedModel();
  GraphModel train(String path, GraphModel structure);
  
  GraphModel getTrueModel();
  void setTrueModel(GraphModel gm);
  
  LogDouble test(String path);
  String toString();
}
