This repo contains tools for extracting features from a binary object files:
1) call_stack: Uses PIN to dynamically extract the call stack for all read instructions.
2) loop_depth: Uses Dyninst to statically extract the loop depth for all read instructions.

Loop depth info needs to be migrated from loop_depth tool to champ_sim tracer to generate traces with both call_stack and loop_depth info.
