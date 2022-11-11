import { IonContent, IonHeader, IonItem, IonLabel, IonList, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../../components/exploreContainer/ExploreContainer';
import './Home.css';

const Home: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Home</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>

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

      </IonContent>
    </IonPage>
  );
};

export default Home;
