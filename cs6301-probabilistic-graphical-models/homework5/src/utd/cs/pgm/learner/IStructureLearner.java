package utd.cs.pgm.learner;

import utd.cs.pgm.core.graphmodel.GraphModel;

public interface IStructureLearner {
  GraphModel getLearnedModel();
  GraphModel train(String path);
  
  String toString();
}
