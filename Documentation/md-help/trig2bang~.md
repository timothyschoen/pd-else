---
title: trig2bang~

description: trigger to bang conversion

categories:
- object

pdcategory: ELSE, Triggers and Clocks

arguments:

inlets:
  1st:
  - type: signal
    description: signal to detect triggers

outlets:
  1st:
  - type: bang
    description: when detecting zero to non-0 transitions

draft: false
---

[trig2bang~] detects zero to non-0 transitions. This can be used to detect and convert trigger signals (impulses and gate) to a bang.
