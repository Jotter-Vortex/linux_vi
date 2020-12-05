# team 12


### vi 구현
###### * linux vi program 구현
    −  1. 기존 vi와 동일한 텍스트 편집기능 제공
    -  2. 키보드 [ 키를 누르면 현재 문단의 제일 왼쪽 끝으로 이동
    -  3. 키보드 ] 키를 누르면 현재 문단의 제일 오른쪽 끝으로 이동
    -  4. 키보드 - 키를 누르면 현재 y위치의 제일 위에 문단으로 이동
    -  5. 키보드 = 키를 누르면 현재 y위치의 제일 아래 문단으로 이동
    -  6. 키보드 H 키를 누르면 윈도우 home키와 같은 기능
    -  7. 키보드 G 키를 누르면 위도우 end키와 같은 기능
    
###### * linux vi program과 차별화된 기능
    −  1. 명령모드에서
          1) :wq -dir dir 
            -> dir이라는 driectory를 만들고 해당하는 dir안에 FILENAME을 가지는 file을 만듬
          2) :wq -t
            -> FILENAME과 test.FILENAME 두개의 file을 만듬
          3) :wq -du myfile
            -> FILENAME이라는 이름을 가지는 file과 myfile이라는 이름을 가진 file을 각각 만듬
