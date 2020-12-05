# team 12


### vi 구현
###### * linux vi program 구현
    −  1. 기존 vi와 동일한 텍스트 편집기능 제공
    
###### * linux vi program과 차별화된 기능
    −  1. 명령모드에서
          1) :wq -dir dir 
            -> dir이라는 driectory를 만들고 해당하는 dir안에 FILENAME을 가지는 file을 만듬
          2) :wq -t
            -> FILENAME과 test.FILENAME 두개의 file을 만듬
          3) :wq -du myfile
            -> FILENAME이라는 이름을 가지는 file과 myfile이라는 이름을 가진 file을 각각 만듬
