classdef KalmanSimple
   properties
      x (2,1) {mustBeNumeric}
      B (2,2) {mustBeNumeric}
      H (1,2) {mustBeNumeric}
   end
   methods
      function obj = KalmanSimple(f0,A,srate,q,r)
          
            obj.f0 = f0;
            obj.A = A;
            obj.H(1,:) = [1,0];
            obj.P(:,:) = [cos(2*pi*f0/srate),-sin(2*pi*f0/srate);
                                                sin(2*pi*f0/srate),cos(2*pi*f0/srate)];
            
            
            obj.x(:,1) = [0,0];
            obj.P(:,:) = eye(2);
            
         end
         
         
      end 
       
       
       
       
      function filtered = roundOff(obj)
         r = obj.x;
         
      end
     
      function step(obj, y) 
           obj.x_ = obj.Phi@obj.x
        
        obj.P_ = obj.Phi@obj.P@obj.Phi.T+obj.Q
        
        obj.res = y-obj.H@obj.x_
        
        obj.S = obj.H@obj.P_@obj.H.T+obj.R
        
        self.K = self.P_@self.H.T/self.S
        
        self.x = self.x_+self.K@self.res
        
        self.P = (np.eye(2)-self.K@self.H)@self.P_
          
      end
   end
end