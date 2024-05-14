module Seg7decode(       // convert active-high 4-bit hex to 7-segment display code, active low
   input [3:0] bin,      // binary input
   output reg [6:0] seg7 // LED cathode outputs,active low, assumes common anode LEDs. Not actually registers. 
    );

 // insert code here

  always @(bin)
    begin
      case(bin)
      4'b0000 :  seg7 = 7'b0000001 ;
      4'b0001 :  seg7 = 7'b1001111 ;
      4'b0010 :  seg7 = 7'b0010010 ;
      4'b0011 :  seg7 = 7'b0000110 ;
      4'b0100 :  seg7 = 7'b1001100 ;
      4'b0101 :  seg7 = 7'b0100100 ;
      4'b0110 :  seg7 = 7'b0100000 ;
      4'b0111 :  seg7 = 7'b0001111;
      4'b1000 :  seg7 = 7'b0000000;
      4'b1001 :  seg7 = 7'b0000100 ;
      4'b1010 :  seg7 = 7'b0001000 ; 
      4'b1011 :  seg7 = 7'b1100000;
      4'b1100 :  seg7 = 7'b0110001 ;
      4'b1101 :  seg7 = 7'b1000010 ;
      4'b1110 :  seg7 = 7'b0110000 ;
      4'b1111 :  seg7 = 7'b0111000 ;

      default: seg7 = 7'b1101010; // "n" ("n" for none, but this will never happen)
      endcase
    end
endmodule