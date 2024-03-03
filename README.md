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
  - 능력치(어트리뷰트 세트)를 게임플레이 이펙트로 초기화. RPG의 복잡한 스탯 공식을 적용
- 게임플레이 태그
  - <img width="599" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/e1f4d0be-76ac-40ea-ba1b-9f47be852593">
  - 능력치(어트리뷰트 세트), 향상된 입력, 게임플레이 이펙트를 태그로 관리
  - <img width="352" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/6ffd3f36-a6a4-4a58-bf0c-99830dcb41c2">
  - 게임 내 태그들을 코드에서는 싱글톤 클래스에서 관리
  - 어트리뷰트 세트의 값이 변경됬을 때 태그를 통해서 UI 수정
- 게임플레이 어빌리티
  - 향상된 입력과 게임플레이 어빌리티의 연결

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
- 향상된 입력

### 프로그래밍
- 인터페이스

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
- 구형 체력, 마나 표시
  - <img width="679" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/2249fc97-6791-4a6a-bb90-eaa661af4568">
  - <img width="891" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/86016547-34b1-4368-88cb-fe9da0707cb8">
  - 체력이나 마나가 닳았을 때 앞의 구슬은 즉시 빠지고, 뒤의 흰색 구슬은 천천히 빠지는 형식으로 구현
- 팝업 위젯 구현
  - <img width="209" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/db79e645-76ec-4e46-aa1a-656edc058945">
  - <img width="1486" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/4d144cfb-8185-4ddf-98f8-9b3767eca5e3">
  - <img width="968" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/efef2774-3bad-42ac-a490-f5a27e8459d6">
  - 게임플레이 이펙트의 태그를 확인해서 태그와 연결된 메시지(데이터 테이블)를 UI에 세팅헤서 출력
- 스탯 메뉴 구현
  - <img width="441" alt="image" src="https://github.com/huzi2/UnrealStudy-Aura/assets/31639085/1e6877a0-b72a-46fc-869b-78e47d2b9a4c">
  - 어트리뷰트 세트와 태그로 연결되어서 어트리뷰트 세트가 변경될 때 바로 적용. 아래 2차 속성은 1차 속성이 변경되면 무한 게임플레이 이펙트로 복잡한 식이 계산되서 바로 적용됨
