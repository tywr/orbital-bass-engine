#pragma once
#include "chorus/chorus_component.h"
#include "ir/ir_component.h"

#include <juce_gui_basics/juce_gui_basics.h>

class PostRackComponent : public juce::Component
{
  public:
    PostRackComponent(juce::AudioProcessorValueTreeState& params);
    ~PostRackComponent() override;

    void resized() override;

  private:
    ChorusComponent chorus_component;
    IRComponent ir_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PostRackComponent)
};
