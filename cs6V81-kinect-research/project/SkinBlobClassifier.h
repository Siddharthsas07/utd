#pragma once

class SkinBlobClassifier {
  public:
    SkinBlobClassifier();
    ~SkinBlobClassifier();

    bool is_skin_color( unsigned int px );
};