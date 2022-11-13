import "./header.css";
import { IonItem, IonLabel, IonList } from "@ionic/react";

const Header: React.FC = () => {
  return (
    <IonList className="listwrapper">
      <IonItem routerLink="login">
        <IonLabel>Login</IonLabel>
      </IonItem>
      <IonItem routerLink="register">
        <IonLabel>Register</IonLabel>
      </IonItem>
      <IonItem routerLink="home">
        <IonLabel>Home</IonLabel>
      </IonItem>
      <IonItem routerLink="fence">
        <IonLabel>Fence</IonLabel>
      </IonItem>
      <IonItem routerLink="map">
        <IonLabel>Map</IonLabel>
      </IonItem>
    </IonList>
  );
};

export default Header;
