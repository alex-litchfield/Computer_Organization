//  Two-way arbiter FSM/HLSM.  
//   Similar to the example in the HLSM chapter, but eliminates extra state ("Arbitrate") and uses meaningful signal names  
module Arbiter(
   input clk,rst,ra,rb, // ra = request from requestor a, rb = request from requestor b, which are level (not pulse) signals
   input [1:0] PA, PB,  // priority values for requestor a and requestor b, unsigned, higher number is higher priority
   output reg ga,gb     // ga = grant to a, gb = grant to b
);
   // insert code here
endmodule