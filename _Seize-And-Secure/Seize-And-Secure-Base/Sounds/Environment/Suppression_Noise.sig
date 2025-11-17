AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 27
   name "Sound_Noise"
   tl -676.364 -655.303
   children {
    28
   }
   global 1
  }
 }
 Ops {
  IOPItemOpConditionClass {
   id 28
   name "Cond Amplitude"
   tl -338.333 -623.333
   children {
    24
   }
   inputs {
    ConnectionClass connection {
     id 27
     port 0
    }
   }
   "Condition Type" ">"
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 24
   name "Suppression_Noise"
   tl -33.75 -595.417
   input 28
  }
 }
 compiled IOPCompiledClass {
  visited {
   5 7 6
  }
  ins {
   IOPCompiledIn {
    data {
     1 3
    }
   }
  }
  ops {
   IOPCompiledOp {
    data {
     1 2 2 0 0
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
  processed 3
  version 2
 }
}