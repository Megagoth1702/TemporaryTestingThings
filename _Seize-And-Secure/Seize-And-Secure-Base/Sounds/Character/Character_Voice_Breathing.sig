AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 27
   name "BreathFrequency"
   tl -131.25 -392.857
   children {
    29
   }
   value 0.55
   global 1
  }
  IOPInputValueClass {
   id 30
   name "0.5"
   tl 83.893 -463.536
   children {
    29
   }
   value 500
  }
 }
 Ops {
  IOPItemOpDivClass {
   id 29
   name "Div 29"
   tl 268.75 -417.679
   children {
    28
   }
   inputs {
    ConnectionClass connection {
     id 30
     port 0
    }
    ConnectionClass connection {
     id 27
     port 1
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 28
   name "Time"
   tl 565.571 -417.464
   input 29
  }
 }
 compiled IOPCompiledClass {
  visited {
   133 5 7 6
  }
  ins {
   IOPCompiledIn {
    data {
     1 3
    }
   }
   IOPCompiledIn {
    data {
     1 3
    }
   }
  }
  ops {
   IOPCompiledOp {
    data {
     1 2 4 65536 0 0 1
    }
   }
  }
  outs {
   IOPCompiledOut {
    data {
     0
    }
   }
  }
  processed 4
  version 2
 }
}