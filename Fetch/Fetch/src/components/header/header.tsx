import './header.css'
import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';

const Header: React.FC = () => {
  return (
      <IonList className="listwrapper">
          <IonItem routerLink="home">
            <IonLabel>Home</IonLabel>
          </IonItem>
          <IonItem routerLink="login">
            <IonLabel>Login</IonLabel>
          </IonItem>
          <IonItem routerLink="register">
            <IonLabel>Register</IonLabel>
          </IonItem>
        </IonList>
  );
};

export default Header;
