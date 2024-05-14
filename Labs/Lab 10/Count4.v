// 4-bit up/down counter
module Count4(clk,rst,enable,upDown, count);
   input clk, rst, enable, upDown;
   output reg [3:0] count = 0;

// insert code here 
      always @ (posedge clk, posedge rst) begin
         if(enable)
            if(rst)
               count <= 0;
            else if(upDown == 1)
               if(count == 15)
                  count <= 0;
               else
                  count <= count + 1;
            else if(upDown == 0)
               if(count == 0)
                  count <= 15;
               else
                  count <= count - 1;
         
      end
endmodule  // Count4