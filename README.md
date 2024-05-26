# UnrealStudy-Aura

<img width="776" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/74afe339-fce5-4d1e-af69-2adaa49bf8dd">

## 프로젝트 개요

- 언리얼 5.3 C++과 GAS를 활용한 쿼터뷰 RPG 게임
  
## 제작 기간

- 24.02.08 ~ (개발 중)

## 개발 환경

- Unreal Engine 5.3
- C++
- 게임플레이 어빌리티 시스템(Gameplay Ability System)
- IDE : Visual Studio 2022

## 사용 기술

### 게임플레이 어빌리티 시스템
- 어빌리티 시스템 컴포넌트
- 어트리뷰트 세트
  - 캐릭터의 능력치를 어트리뷰트 세트로 구현. 레플리케이션을 활용해서 멀티플레이 환경을 생각하면서 구현
  - 어트리뷰트 세트를 변경할 때는 게임플레이 이펙트를 사용
- 게임플레이 이펙트
  - 즉시(포션), 지속(포션), 무한(불지역)의 다양한 효과를 가진 게임플레이 이펙트가 적용된 액터들을 구현
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/caefaea0-f7aa-4fdc-a7cb-e585f97b2b26)
  - 능력치(어트리뷰트 세트)를 게임플레이 이펙트로 초기화. RPG의 복잡한 스탯 공식을 적용
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/778bd31c-3257-4e17-a2b1-ccfff8475406)
  - 게임플레이 이펙트와 커브 테이블을 통해서 레벨에 따라 능력치가 달라지도록 적용
- 게임플레이 태그
  - <img width="599" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/e1f4d0be-76ac-40ea-ba1b-9f47be852593">
  - 능력치(어트리뷰트 세트), 향상된 입력, 게임플레이 이펙트를 태그로 관리
  - <img width="352" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/6ffd3f36-a6a4-4a58-bf0c-99830dcb41c2">
  - 게임 내 태그들을 코드에서는 싱글톤 클래스에서 관리
  - 어트리뷰트 세트의 값이 변경됬을 때 태그를 통해서 UI 수정
- 게임플레이 어빌리티
  - 향상된 입력과 게임플레이 어빌리티의 연결
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/642dac9e-9005-43f6-b267-ba6ed058fa32)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/a3e2c806-e882-4ce8-b07c-a8dfa6d1205c)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/c9dc21ce-ef51-4cce-bc25-e2dd8b55b080)
  - 클릭으로 적에게 화염구를 날리는 어빌리티의 구현
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/afde80ff-978c-4cdf-b0b4-f8702faaf067)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/05539f47-8727-4e39-b66e-2c3d5fcefa4a)
  - 피격 반응을 어빌리티로 구현
  - <img width="782" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/88975087-64d3-4a4b-bb07-560c7d4865fa">
  - <img width="509" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/99c75645-e51b-4e3a-be0a-5329bdc92e22">
  - <img width="779" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/5d4360c4-7c0d-44f0-a9e8-628277a83c48">
  - <img width="1187" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/38a01b2b-ebf0-4a63-b03b-3351d8afb3c2">
  - <img width="1176" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/a60b49dc-6b38-4531-9352-657838952dd2">
  - 몬스터의 근접 평타를 어빌리티로 구현
  - - <img width="809" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/fb38b44a-0dc0-44e7-8ce8-fb65201755c8">
  - 몬스터의 원거리 평타를 어빌리티로 구현
  - <img width="1122" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/4af2b1ec-7129-4092-8171-44ce0dfc9182">
  - 몬스터의 소환 능력을 어빌리티로 구현
- 게임플레이 큐
  - <img width="1180" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/6698ed4e-cbf9-4115-b8aa-f9fe2a2b4c3d">
  - 사운드와 이펙트의 재생은 게임플레이 큐를 통해서 수행. 어빌리티는 서버에서 수행되므로, 다른 클라이언트에서도 이펙트를 보기 위함
- 실행 계산(UGameplayEffectExecutionCalculation)으로 복잡한 데미지 처리 구현
  - 이펙트를 수행한 소스의 크리티컬 어트리뷰트로 확률을 계산해서 데미지를 올리고, 타겟의 블록 어트리뷰트로 데미지를 줄이는 등, 복잡한 데미지를 실행 계산으로 구현

### 게임 로직
- 마우스에 닿은 적 강조
  - <img width="250" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/31be81cf-1730-413d-b28c-22b7a83a43bb">
- 포션, 불지역 등의 RPG 식 오브젝트 구현
  - <img width="889" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/8588ebfa-295a-4951-920c-788c5136cdf9">
  - <img width="448" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/d89c12e6-d075-4e67-89ab-437d597a214b">
  - <img width="364" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/c1e1a19b-57ac-40be-bc5d-71d0606ecf1b">
  - 효과는 게임플레이 이펙트로 구현. 어트리뷰트 세트에 효과 적용
- RPG 식 능력치 구현
  - <img width="442" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/b6c51d9e-683e-49df-9521-bf8731668324">
  - 방어력 = (회복력 + 2) * 0.25 + 6 식으로 복잡한 공식의 능력치 형태를 게임플레이 이펙트로 구현
  - Infinite로 적용되어서 스탯이 변경되면 즉시 공식 적용됨
- 몬스터의 공격 구현
  - <img width="409" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/ac71107d-79a9-4f41-89d3-cfb82fa15854">
  - 몬스터마다 사용할 공격 모션의 몽타주와 몽타주 태그, 공격 지점을 정할 소켓 태그, 사용할 사운드를 지정
  - <img width="745" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/bb5fb33f-09b6-4f3b-8f79-2028042b66df">
  - 커스텀 애님 노티파이에서 액터에게 태그를 통한 게임플레이 이벤트를 적용
  - <img width="741" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/dc7fa807-ee34-439b-958d-2628d0551e24">
  - 공격 모션 몽타주에서 타격 지점에서 위 태그와 같이 애님 노티파이를 수행
  - <img width="785" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/d809c76f-3051-4045-b14d-4be58c0cf058">
  - 공격 어빌리티에서 몽타주를 실행하고 비동기로 이벤트를 대기
  - <img width="1179" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/e33820e8-15c6-4613-a3ed-ce3334f5e07e">
  - 이벤트가 수행되면 소켓 태그를 확인해서 소켓 주변에 있는 타겟들에게 공격 데미지 이펙트를 적용
- 페이드 액터 구현
  - <img width="607" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/8ef61ba1-75ce-4f0b-b798-22eee34de8f7">
  - <img width="590" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/c5d9826b-2348-405f-94ec-ef8c7582d163">

### 프로그래밍
- 인터페이스

### AI
- ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/95a8af08-062a-44a1-a79e-355f226d2caa)
- 비헤이비어 트리
- 블랙보드
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/58aa6140-5e13-4f86-886a-d2d59e0960ce)
  - 근접 몬스터의 AI 구현
  - <img width="217" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/b9ce83ff-3fec-4ae6-8551-8b464c58f422">
  - 원거리 몬스터의 AI 구현
  - <img width="307" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/b022792a-2ae8-4ae8-ac49-73eb0e4a9efa">
  - 소환 몬스터의 AI 구현
- EQS
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/105273ef-f9dc-4150-9c33-aba302414d98)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/8f7b5c9d-9148-49a7-aae9-e6a04eb08a1d)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/75d9a67c-af54-458c-9107-8d0e33ee37b2)
  - 벽 뒤의 플레이어를 공격하기 위한 위치를 찾기 위해 EQS 사용

### UI
- 어트리뷰트 세트(데이터) <=> 위젯 컨트롤러 <=> 위젯(UI) 구조로 데이터가 변경되었을 때 위젯 컨트롤러를 통해서 태그와 연결된 UI에 데이터를 반영
  - <img width="1124" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/bc66234e-bdeb-489f-b5d9-92208996f069">
  - 스탯 관련 태그를 준비
  - <img width="566" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/ad36bccd-721c-46ee-b784-996d6d2bea54">
  - 어트리뷰트 세트의 GetAttribute() 함수와 태그를 연결
  - <img width="387" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/8f160b1c-f694-4882-831b-fbdc0d44114d">
  - UI에 태그 설정
  - <img width="476" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/a9323563-b722-461f-a6fc-f0ad5eda23bf">
  - UI의 태그에 따라 GetGameplayAttributeValueChangeDelegate()에 값 변경을 바인드. 해당 태그의 어트리뷰트 세트가 변경되면 같은 태그의 UI의 값이 바로 적용된다.
- 전체적인 HUD 구성
  - <img width="1517" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/b3e3950e-c98e-45ce-90dd-ea043162e835">
- 구형 체력, 마나 표시
  - <img width="679" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/2249fc97-6791-4a6a-bb90-eaa661af4568">
  - <img width="891" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/86016547-34b1-4368-88cb-fe9da0707cb8">
  - 체력이나 마나가 닳았을 때 앞의 구슬은 즉시 빠지고, 뒤의 흰색 구슬은 천천히 빠지는 형식으로 구현
- 능력치 메뉴 구현
  - <img width="439" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/1327dd2f-4783-47c9-9052-afb25798cea9">
  - 어트리뷰트 세트와 태그로 연결되어서 어트리뷰트 세트가 변경될 때 바로 적용. 아래 2차 속성은 1차 속성이 변경되면 무한 게임플레이 이펙트로 복잡한 식이 계산되서 바로 적용됨
- 스킬 메뉴 구현
  - <img width="436" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/30cbe446-7b40-475b-a0cd-956021d83515">
- 팝업 위젯 구현
  - <img width="209" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/db79e645-76ec-4e46-aa1a-656edc058945">
  - <img width="1486" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/4d144cfb-8185-4ddf-98f8-9b3767eca5e3">
  - <img width="968" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/efef2774-3bad-42ac-a490-f5a27e8459d6">
  - 게임플레이 이펙트의 태그를 확인해서 태그와 연결된 메시지(데이터 테이블)를 UI에 세팅헤서 출력
- 적 체력바와 데미지 팝업 위젯 구현
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/26172a44-dac3-4e58-8bea-69a97f09e150)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/6be13e01-03f1-4004-aa08-26776904c228)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/4aec3f20-2568-4272-bd79-bd76cacb7843)
  - ![image](https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/ee1e665c-4b13-4cde-bbc9-ad0e851e5abe)
  - 치명타, 블록 등의 데미지 상태를 실행 계산으로 구현해서 데미지 팝업에 적용
